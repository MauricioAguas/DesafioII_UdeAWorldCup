#include "Partido.h"
#include <iostream>
#include <cstdlib>
using namespace std;

Partido::Partido() {
    fecha    = "";
    hora     = "";
    sede     = "";
    equipo1  = nullptr;
    equipo2  = nullptr;
    resultado = nullptr;
    codsArbitros[0] = "codArbitro1";
    codsArbitros[1] = "codArbitro2";
    codsArbitros[2] = "codArbitro3";
}

Partido::Partido(string fecha, string hora, string sede, Equipo* e1, Equipo* e2) {
    this->fecha   = fecha;
    this->hora    = hora;
    this->sede    = sede;
    this->equipo1 = e1;
    this->equipo2 = e2;
    resultado     = nullptr;
    codsArbitros[0] = "codArbitro1";
    codsArbitros[1] = "codArbitro2";
    codsArbitros[2] = "codArbitro3";
}

Partido::~Partido() {
    if (resultado != nullptr) delete resultado;
}

// Ecuacion del enunciado: lambda = 0.4*promGFA + 0.6*promGEC + 1.35
float Partido::calcularGolesEsperados(Equipo* ataque, Equipo* defensa) {
    const float alpha = 0.4f;
    const float beta  = 0.6f;
    const float gamma = 1.35f;
    float promGFA = (ataque->getGFA() > 0)  ? (float)ataque->getGFA()  : 1.0f;
    float promGEC = (defensa->getGEC() > 0) ? (float)defensa->getGEC() : 1.0f;
    return alpha * promGFA + beta * promGEC + gamma;
}

static void distribuirGoles(Jugador** convocados, int totalGoles) {
    int golesAsignados = 0;
    for (int pasada = 0; pasada < 3 && golesAsignados < totalGoles; pasada++) {
        for (int i = 0; i < 11 && golesAsignados < totalGoles; i++) {
            if ((rand() % 100) < 4) {
                convocados[i]->actualizarEstadisticas(1, 0, 0, 0, 0);
                golesAsignados++;
            }
        }
    }
    while (golesAsignados < totalGoles) {
        convocados[0]->actualizarEstadisticas(1, 0, 0, 0, 0);
        golesAsignados++;
    }
}

static void asignarTarjetasYFaltas(Jugador** convocados) {
    for (int i = 0; i < 11; i++) {
        int faltas = 0;
        if ((rand() % 10000) < 1300) {
            faltas++;
            if ((rand() % 10000) < 275) {
                faltas++;
                if ((rand() % 10000) < 70) faltas++;
            }
        }
        int amarillas = 0;
        if ((rand() % 10000) < 600) {
            amarillas++;
            if ((rand() % 10000) < 115) amarillas++;
        }
        int rojas = (amarillas >= 2) ? 1 : 0;
        if (faltas > 0 || amarillas > 0)
            convocados[i]->actualizarEstadisticas(0, 0, amarillas, rojas, faltas);
    }
}

void Partido::simular() {
    Jugador** conv1 = equipo1->seleccionarConvocados();
    Jugador** conv2 = equipo2->seleccionarConvocados();

    float lambda1 = calcularGolesEsperados(equipo1, equipo2);
    float lambda2 = calcularGolesEsperados(equipo2, equipo1);

    int gf1 = (int)(lambda1 * ((rand() % 200) / 100.0f));
    int gf2 = (int)(lambda2 * ((rand() % 200) / 100.0f));

    resultado = new Resultado(gf1, gf2, false, conv1, conv2);
    resultado->calcularPosesion(equipo1->getRanking(), equipo2->getRanking());

    asignarGoleadores();
    asignarTarjetasYFaltas(conv1);
    asignarTarjetasYFaltas(conv2);
    resultado->actualizarHistoricos(equipo1, equipo2);

    delete[] conv1;
    delete[] conv2;
}

// Para eliminatorias: si hay empate, sesgar ganador por ranking y sumar 1 gol
void Partido::simularConProrroga() {
    simular();
    if (resultado->getGfEquipo1() == resultado->getGfEquipo2()) {
        // Sesgar por ranking: menor ranking = mejor equipo = mas probabilidad
        int r1 = equipo1->getRanking();
        int r2 = equipo2->getRanking();
        // prob de que gane e1 = r2 / (r1+r2)
        int total = r1 + r2;
        int umbral = (total > 0) ? (r2 * 100 / total) : 50;
        // Marcar prorroga y ajustar goles
        delete resultado;
        Jugador** conv1 = equipo1->seleccionarConvocados();
        Jugador** conv2 = equipo2->seleccionarConvocados();
        int gf1 = resultado ? resultado->getGfEquipo1() : 1;
        int gf2 = resultado ? resultado->getGfEquipo2() : 1;
        // Sumar 1 gol al ganador sesgado
        if ((rand() % 100) < umbral) gf1++;
        else                          gf2++;
        resultado = new Resultado(gf1, gf2, true, conv1, conv2);
        resultado->calcularPosesion(r1, r2);
        resultado->actualizarHistoricos(equipo1, equipo2);
        delete[] conv1;
        delete[] conv2;
    }
}

void Partido::asignarGoleadores() {
    if (resultado == nullptr) return;
    distribuirGoles(resultado->getConvocados1(), resultado->getGfEquipo1());
    distribuirGoles(resultado->getConvocados2(), resultado->getGfEquipo2());
}

void Partido::imprimirResumen() {
    cout << "\n=== Partido ===";
    cout << "\nFecha: " << fecha << "  Hora: " << hora << "  Sede: " << sede;
    cout << "\n" << equipo1->getPais() << " " << resultado->getGfEquipo1()
         << " - " << resultado->getGfEquipo2() << " " << equipo2->getPais();
    if (resultado->getHuboProrroga()) cout << " (Prorroga)";
    cout << "\nGoleadores " << equipo1->getPais() << ": ";
    for (int i = 0; i < 11; i++) {
        Jugador* j = resultado->getConvocados1()[i];
        if (j != nullptr && j->getGoles() > 0) cout << j->getNumeroCamiseta() << " ";
    }
    cout << "\nGoleadores " << equipo2->getPais() << ": ";
    for (int i = 0; i < 11; i++) {
        Jugador* j = resultado->getConvocados2()[i];
        if (j != nullptr && j->getGoles() > 0) cout << j->getNumeroCamiseta() << " ";
    }
    cout << endl;
}

Resultado* Partido::getResultado()  { return resultado; }
Equipo*    Partido::getEquipo1()    { return equipo1; }
Equipo*    Partido::getEquipo2()    { return equipo2; }
string     Partido::getFecha()      { return fecha; }

ostream& operator<<(ostream& os, const Partido& p) {
    os << p.fecha << " " << p.hora << " | "
       << p.equipo1->getPais() << " vs " << p.equipo2->getPais()
       << " | Sede: " << p.sede;
    return os;
}
