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
    // Los punteros convocados son propiedad del Equipo, no se eliminan aqui
}

// Ecuacion de goles esperados segun enunciado:
// lambda = alpha * promGFA_A + beta * promGEC_B + gamma
// alpha=0.4, beta=0.6, gamma=1.35  (valores del enunciado)
float Partido::calcularGolesEsperados(Equipo* ataque, Equipo* defensa) {
    const float alpha = 0.4f;
    const float beta  = 0.6f;
    const float gamma = 1.35f;

    // Si el equipo no tiene partidos jugados aun, usamos 1.0 como promedio base
    float promGFA = (ataque->getGFA() > 0)  ? (float)ataque->getGFA()  : 1.0f;
    float promGEC = (defensa->getGEC() > 0) ? (float)defensa->getGEC() : 1.0f;

    float lambda = alpha * promGFA + beta * promGEC + gamma;
    return lambda;
}

// Distribuye goles entre los 11 convocados: prob 4% por jugador hasta
// alcanzar el total de goles del equipo, segun el enunciado
static void distribuirGoles(Jugador** convocados, int totalGoles) {
    int golesAsignados = 0;
    // Hasta 3 pasadas para intentar cubrir todos los goles
    for (int pasada = 0; pasada < 3 && golesAsignados < totalGoles; pasada++) {
        for (int i = 0; i < 11 && golesAsignados < totalGoles; i++) {
            int r = rand() % 100;
            if (r < 4) { // 4% de probabilidad
                convocados[i]->actualizarEstadisticas(1, 0, 0, 0, 0);
                golesAsignados++;
            }
        }
    }
    // Si aun faltan goles, asignarlos al jugador 0
    while (golesAsignados < totalGoles) {
        convocados[0]->actualizarEstadisticas(1, 0, 0, 0, 0);
        golesAsignados++;
    }
}

// Asigna tarjetas y faltas segun probabilidades del enunciado
static void asignarTarjetasYFaltas(Jugador** convocados) {
    for (int i = 0; i < 11; i++) {
        // Faltas: 1a falta 13%, 2a 2.75%, 3a 0.7%
        int faltas = 0;
        if ((rand() % 10000) < 1300) {
            faltas++;
            if ((rand() % 10000) < 275) {
                faltas++;
                if ((rand() % 10000) < 70)
                    faltas++;
            }
        }
        // Amarillas: 6% primera, 1.15% segunda
        int amarillas = 0;
        if ((rand() % 10000) < 600) {
            amarillas++;
            if ((rand() % 10000) < 115)
                amarillas++;
        }
        // Roja: solo si tiene dos amarillas
        int rojas = (amarillas >= 2) ? 1 : 0;

        if (faltas > 0 || amarillas > 0)
            convocados[i]->actualizarEstadisticas(0, 0, amarillas, rojas, faltas);
    }
}

void Partido::simular() {
    // Seleccionar 11 convocados de cada equipo
    Jugador** conv1 = equipo1->seleccionarConvocados();
    Jugador** conv2 = equipo2->seleccionarConvocados();

    // Calcular goles esperados (lambda de Poisson simplificado: truncar a entero)
    float lambda1 = calcularGolesEsperados(equipo1, equipo2);
    float lambda2 = calcularGolesEsperados(equipo2, equipo1);

    // Simular goles: muestreo Poisson simplificado con rand()
    int gf1 = 0, gf2 = 0;
    // Usamos aprox: goles = floor(lambda * U) con U en [0,2]
    gf1 = (int)(lambda1 * ((rand() % 200) / 100.0f));
    gf2 = (int)(lambda2 * ((rand() % 200) / 100.0f));

    bool prorroga = false;

    // Crear resultado con goles y convocados
    resultado = new Resultado(gf1, gf2, prorroga, conv1, conv2);

    // Calcular posesion
    resultado->calcularPosesion(equipo1->getRanking(), equipo2->getRanking());

    // Distribuir goles entre convocados
    asignarGoleadores();

    // Asignar tarjetas y faltas
    asignarTarjetasYFaltas(conv1);
    asignarTarjetasYFaltas(conv2);

    // Actualizar historicos de equipos y jugadores (minutos jugados)
    resultado->actualizarHistoricos(equipo1, equipo2);

    delete[] conv1;
    delete[] conv2;
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
    if (resultado->getHuboProrroga())
        cout << " (Prorroga)";
    cout << "\nGoleadores " << equipo1->getPais() << ": ";
    for (int i = 0; i < 11; i++) {
        Jugador* j = resultado->getConvocados1()[i];
        if (j != nullptr && j->getGoles() > 0)
            cout << j->getNumeroCamiseta() << " ";
    }
    cout << "\nGoleadores " << equipo2->getPais() << ": ";
    for (int i = 0; i < 11; i++) {
        Jugador* j = resultado->getConvocados2()[i];
        if (j != nullptr && j->getGoles() > 0)
            cout << j->getNumeroCamiseta() << " ";
    }
    cout << endl;
}

ostream& operator<<(ostream& os, const Partido& p) {
    os << p.fecha << " " << p.hora << " | "
       << p.equipo1->getPais() << " vs " << p.equipo2->getPais()
       << " | Sede: " << p.sede;
    return os;
}
