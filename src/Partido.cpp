#include "../hds/Partido.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

Partido::Partido() {
    fecha = hora = sede = "";
    equipo1 = equipo2 = nullptr;
    resultado = nullptr;
    codsArbitros[0] = "codArbitro1";
    codsArbitros[1] = "codArbitro2";
    codsArbitros[2] = "codArbitro3";
}

Partido::Partido(string fecha, string hora, string sede, Equipo* e1, Equipo* e2) {
    this->fecha = fecha; this->hora = hora; this->sede = sede;
    this->equipo1 = e1; this->equipo2 = e2;
    resultado = nullptr;
    codsArbitros[0] = "codArbitro1";
    codsArbitros[1] = "codArbitro2";
    codsArbitros[2] = "codArbitro3";
}

Partido::~Partido() { if (resultado) delete resultado; }

// Formula del enunciado (ecuacion 1):
// lambda_A = mu * (GF_A / mu)^alpha * (GC_B / mu)^beta
// alpha = 0.6, beta = 0.4, mu = 1.35
float Partido::calcularGolesEsperados(Equipo* ataque, Equipo* defensa) {
    const float mu    = 1.35f;
    const float alpha = 0.6f;
    const float beta  = 0.4f;

    int pjA = ataque->getPartidosJugados();
    int pjD = defensa->getPartidosJugados();

    float gfa = (pjA > 0) ? (float)ataque->getGFA() / pjA : mu;
    float gcb = (pjD > 0) ? (float)defensa->getGEC() / pjD : mu;

    if (gfa < 0.01f) gfa = 0.01f;
    if (gcb < 0.01f) gcb = 0.01f;

    float lambda = mu * pow(gfa / mu, alpha) * pow(gcb / mu, beta);
    return lambda;
}

static void distribuirGoles(Jugador** conv, int total, Resultado* res, bool esEquipo1) {
    int asignados = 0;
    for (int p = 0; p < 5 && asignados < total; p++) {
        for (int i = 0; i < 11 && asignados < total; i++) {
            if ((rand() % 100) < 4) {
                conv[i]->actualizarEstadisticas(1, 0, 0, 0, 0);
                if (esEquipo1) res->registrarGolPartido1(i);
                else           res->registrarGolPartido2(i);
                asignados++;
            }
        }
    }
    while (asignados < total) {
        conv[0]->actualizarEstadisticas(1, 0, 0, 0, 0);
        if (esEquipo1) res->registrarGolPartido1(0);
        else           res->registrarGolPartido2(0);
        asignados++;
    }
}

static void asignarTarjetasYFaltas(Jugador** conv) {
    for (int i = 0; i < 11; i++) {
        int f = 0, am = 0;
        if ((rand()%10000) < 1300) {
            f++;
            if ((rand()%10000) < 275) {
                f++;
                if ((rand()%10000) < 70) f++;
            }
        }
        if ((rand()%10000) < 600)  { am++; if ((rand()%10000) < 115) am++; }
        int ro = (am >= 2) ? 1 : 0;
        if (f > 0 || am > 0)
            conv[i]->actualizarEstadisticas(0, 0, am, ro, f);
    }
}

// simularInterno: calcula marcador y convocados pero NO actualiza historicos.
// Retorna c1 y c2 por referencia para que el caller los libere.
static Resultado* simularInterno(Equipo* equipo1, Equipo* equipo2,
                                  Jugador**& c1, Jugador**& c2,
                                  bool forzarDesempate = false) {
    c1 = equipo1->seleccionarConvocados();
    c2 = equipo2->seleccionarConvocados();

    float lambda1, lambda2;
    if (forzarDesempate) {
        lambda1 = lambda2 = 0.5f;
    } else {
        const float mu = 1.35f, alpha = 0.6f, beta = 0.4f;
        auto golesEsp = [&](Equipo* at, Equipo* def) -> float {
            int pjA = at->getPartidosJugados();
            int pjD = def->getPartidosJugados();
            float gfa = (pjA > 0) ? (float)at->getGFA()  / pjA : mu;
            float gcb = (pjD > 0) ? (float)def->getGEC() / pjD : mu;
            if (gfa < 0.01f) gfa = 0.01f;
            if (gcb < 0.01f) gcb = 0.01f;
            return mu * pow(gfa / mu, alpha) * pow(gcb / mu, beta);
        };
        lambda1 = golesEsp(equipo1, equipo2);
        lambda2 = golesEsp(equipo2, equipo1);
    }

    int gf1, gf2;
    if (forzarDesempate) {
        int r1 = equipo1->getRanking(), r2 = equipo2->getRanking();
        int umbral = (r1 + r2 > 0) ? (r2 * 100 / (r1 + r2)) : 50;
        gf1 = 0; gf2 = 0;
        if ((rand() % 100) < umbral) gf1 = 1; else gf2 = 1;
    } else {
        gf1 = (int)(lambda1 + 0.5f) + (rand() % 3) - 1;
        gf2 = (int)(lambda2 + 0.5f) + (rand() % 3) - 1;
        if (gf1 < 0) gf1 = 0;
        if (gf2 < 0) gf2 = 0;
    }

    Resultado* res = new Resultado(gf1, gf2, forzarDesempate, c1, c2);
    res->calcularPosesion(equipo1->getRanking(), equipo2->getRanking());
    distribuirGoles(c1, gf1, res, true);
    distribuirGoles(c2, gf2, res, false);
    asignarTarjetasYFaltas(c1);
    asignarTarjetasYFaltas(c2);
    return res;
}

void Partido::simular() {
    // FIX: liberar resultado previo para evitar double-free si se llama dos veces
    if (resultado) {
        delete resultado;
        resultado = nullptr;
    }

    Jugador** conv1 = equipo1->seleccionarConvocados();
    Jugador** conv2 = equipo2->seleccionarConvocados();

    float lambda1 = calcularGolesEsperados(equipo1, equipo2);
    float lambda2 = calcularGolesEsperados(equipo2, equipo1);

    int gf1 = (int)(lambda1 + 0.5f) + (rand() % 3) - 1;
    int gf2 = (int)(lambda2 + 0.5f) + (rand() % 3) - 1;
    if (gf1 < 0) gf1 = 0;
    if (gf2 < 0) gf2 = 0;

    resultado = new Resultado(gf1, gf2, false, conv1, conv2);
    resultado->calcularPosesion(equipo1->getRanking(), equipo2->getRanking());
    distribuirGoles(conv1, gf1, resultado, true);
    distribuirGoles(conv2, gf2, resultado, false);
    asignarTarjetasYFaltas(conv1);
    asignarTarjetasYFaltas(conv2);
    resultado->actualizarHistoricos(equipo1, equipo2);
    delete[] conv1;
    delete[] conv2;
}

// FIX: simularConProrroga no llama simular() internamente.
// Hace todo en un solo flujo para evitar doble actualizacion de historicos.
void Partido::simularConProrroga() {
    // FIX: liberar resultado previo para consistencia con simular()
    if (resultado) {
        delete resultado;
        resultado = nullptr;
    }

    Jugador** conv1 = equipo1->seleccionarConvocados();
    Jugador** conv2 = equipo2->seleccionarConvocados();

    float lambda1 = calcularGolesEsperados(equipo1, equipo2);
    float lambda2 = calcularGolesEsperados(equipo2, equipo1);

    int gf1 = (int)(lambda1 + 0.5f) + (rand() % 3) - 1;
    int gf2 = (int)(lambda2 + 0.5f) + (rand() % 3) - 1;
    if (gf1 < 0) gf1 = 0;
    if (gf2 < 0) gf2 = 0;

    bool prorroga = (gf1 == gf2);
    if (prorroga) {
        int r1 = equipo1->getRanking(), r2 = equipo2->getRanking();
        int umbral = (r1 + r2 > 0) ? (r2 * 100 / (r1 + r2)) : 50;
        if ((rand() % 100) < umbral) gf1++; else gf2++;
    }

    resultado = new Resultado(gf1, gf2, prorroga, conv1, conv2);
    resultado->calcularPosesion(equipo1->getRanking(), equipo2->getRanking());
    distribuirGoles(conv1, gf1, resultado, true);
    distribuirGoles(conv2, gf2, resultado, false);
    asignarTarjetasYFaltas(conv1);
    asignarTarjetasYFaltas(conv2);
    resultado->actualizarHistoricos(equipo1, equipo2);
    delete[] conv1;
    delete[] conv2;
}

void Partido::imprimirResumen() {
    // FIX: guards para equipo1/equipo2 y resultado nullptr
    if (!equipo1 || !equipo2) return;
    if (!resultado) {
        cout << "\n=== Partido ===";
        cout << "\nFecha: " << fecha << "  Hora: " << hora << "  Sede: " << sede;
        cout << "\nArbitros: " << codsArbitros[0] << ", " << codsArbitros[1] << ", " << codsArbitros[2];
        cout << "\n" << equipo1->getPais() << " vs " << equipo2->getPais() << " (Sin resultado)\n";
        return;
    }

    cout << "\n=== Partido ===";
    cout << "\nFecha: " << fecha << "  Hora: " << hora << "  Sede: " << sede;
    cout << "\nArbitros: " << codsArbitros[0] << ", " << codsArbitros[1] << ", " << codsArbitros[2];
    cout << "\n" << equipo1->getPais() << " " << resultado->getGfEquipo1()
         << " - " << resultado->getGfEquipo2() << " " << equipo2->getPais();
    if (resultado->getHuboProrroga()) cout << " (Prorroga)";

    int pos1 = (int)(resultado->getPosesionEq1() * 100.0f);
    int pos2 = 100 - pos1;
    cout << "\nPosesion: " << equipo1->getPais() << " " << pos1
         << "%  |  " << equipo2->getPais() << " " << pos2 << "%";

    cout << "\nGoleadores " << equipo1->getPais() << ": ";
    bool hayGol1 = false;
    for (int i = 0; i < 11; i++) {
        // FIX: guard para convocado nullptr antes de acceder a getNumeroCamiseta
        Jugador* j = resultado->getConvocados1()[i];
        if (j && resultado->getGolesPartido1(i) > 0) {
            cout << j->getNumeroCamiseta()
                 << "(" << resultado->getGolesPartido1(i) << ") ";
            hayGol1 = true;
        }
    }
    if (!hayGol1) cout << "ninguno";

    cout << "\nGoleadores " << equipo2->getPais() << ": ";
    bool hayGol2 = false;
    for (int i = 0; i < 11; i++) {
        // FIX: guard para convocado nullptr
        Jugador* j = resultado->getConvocados2()[i];
        if (j && resultado->getGolesPartido2(i) > 0) {
            cout << j->getNumeroCamiseta()
                 << "(" << resultado->getGolesPartido2(i) << ") ";
            hayGol2 = true;
        }
    }
    if (!hayGol2) cout << "ninguno";
    cout << endl;
}

Resultado* Partido::getResultado() const { return resultado; }
Equipo*    Partido::getEquipo1()   { return equipo1; }
Equipo*    Partido::getEquipo2()   { return equipo2; }
string     Partido::getFecha()     { return fecha; }

ostream& operator<<(ostream& os, const Partido& p) {
    os << p.fecha << " " << p.hora << " | "
       << p.equipo1->getPais() << " vs " << p.equipo2->getPais()
       << " | Sede: " << p.sede;
    return os;
}
