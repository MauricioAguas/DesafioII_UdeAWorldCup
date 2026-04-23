#include "../include/Resultado.h"
#include <iostream>
using namespace std;

Resultado::Resultado() {
    gfEquipo1 = gfEquipo2 = 0;
    posesionEq1  = 0.5f;
    huboProrroga = false;
    for (int i = 0; i < 11; i++) { convocados1[i] = nullptr; convocados2[i] = nullptr; }
}

Resultado::Resultado(int gf1, int gf2, bool prorroga, Jugador** conv1, Jugador** conv2) {
    gfEquipo1    = gf1;
    gfEquipo2    = gf2;
    huboProrroga = prorroga;
    posesionEq1  = 0.5f;
    for (int i = 0; i < 11; i++) { convocados1[i] = conv1[i]; convocados2[i] = conv2[i]; }
}

Resultado::~Resultado() {}

void Resultado::calcularPosesion(int rankA, int rankB) {
    if (rankA + rankB == 0) { posesionEq1 = 0.5f; return; }
    posesionEq1 = (float)rankB / (float)(rankA + rankB);
    if (posesionEq1 < 0.35f) posesionEq1 = 0.35f;
    if (posesionEq1 > 0.65f) posesionEq1 = 0.65f;
}

Equipo* Resultado::getGanador(Equipo* e1, Equipo* e2) {
    if (gfEquipo1 > gfEquipo2) return e1;
    if (gfEquipo2 > gfEquipo1) return e2;
    return nullptr;
}

void Resultado::actualizarHistoricos(Equipo* e1, Equipo* e2) {
    int resE1 = (gfEquipo1 > gfEquipo2) ? 1 : (gfEquipo1 < gfEquipo2) ? -1 : 0;
    int resE2 = -resE1;
    e1->actualizarEstadisticas(gfEquipo1, gfEquipo2, resE1);
    e2->actualizarEstadisticas(gfEquipo2, gfEquipo1, resE2);
    int minutos = huboProrroga ? 120 : 90;
    for (int i = 0; i < 11; i++) {
        if (convocados1[i]) convocados1[i]->actualizarEstadisticas(0, minutos, 0, 0, 0);
        if (convocados2[i]) convocados2[i]->actualizarEstadisticas(0, minutos, 0, 0, 0);
    }
}

int      Resultado::getGfEquipo1()     { return gfEquipo1; }
int      Resultado::getGfEquipo2()     { return gfEquipo2; }
bool     Resultado::getHuboProrroga()  { return huboProrroga; }
Jugador** Resultado::getConvocados1()  { return convocados1; }
Jugador** Resultado::getConvocados2()  { return convocados2; }
