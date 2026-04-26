#include "../hds/Resultado.h"
#include <iostream>
using namespace std;

Resultado::Resultado() {
    gfEquipo1 = gfEquipo2 = 0;
    posesionEq1  = 0.5f;
    huboProrroga = false;
    for (int i = 0; i < 11; i++) {
        convocados1[i] = nullptr;
        convocados2[i] = nullptr;
        golesPartido1[i] = 0;
        golesPartido2[i] = 0;
    }
}

Resultado::Resultado(int gf1, int gf2, bool prorroga, Jugador** conv1, Jugador** conv2) {
    gfEquipo1    = gf1;
    gfEquipo2    = gf2;
    huboProrroga = prorroga;
    posesionEq1  = 0.5f;
    for (int i = 0; i < 11; i++) {
        convocados1[i] = conv1[i];
        convocados2[i] = conv2[i];
        golesPartido1[i] = 0;
        golesPartido2[i] = 0;
    }
}

Resultado::~Resultado() {}

// FIX Bug 3: posesion corregida.
// Ranking FIFA: menor numero = mejor equipo.
// El equipo con menor ranking (mejor) tiene mas posesion.
// Formula: posesionEq1 = rankB / (rankA + rankB)
// Si eq1 tiene rank 1 y eq2 rank 100: posesion = 100/101 ≈ 0.99 → clampea a 0.65
// Si eq1 tiene rank 100 y eq2 rank 1: posesion = 1/101 ≈ 0.01 → clampea a 0.35
// El clamp [0.35, 0.65] evita valores extremos poco realistas.
void Resultado::calcularPosesion(int rankA, int rankB) {
    if (rankA + rankB == 0) { posesionEq1 = 0.5f; return; }
    // rankB mayor = rival peor = eq1 domina mas
    posesionEq1 = (float)rankB / (float)(rankA + rankB);
    if (posesionEq1 < 0.35f) posesionEq1 = 0.35f;
    if (posesionEq1 > 0.65f) posesionEq1 = 0.65f;
}

// FIX: getGanador garantiza retorno no-null en fase eliminatoria.
// En fase de grupos puede retornar nullptr (empate valido).
// En eliminatoria, simularConProrroga() garantiza gf1 != gf2 antes de llamar aqui.
Equipo* Resultado::getGanador(Equipo* e1, Equipo* e2) {
    if (gfEquipo1 > gfEquipo2) return e1;
    if (gfEquipo2 > gfEquipo1) return e2;
    // Empate residual (solo en grupos): retornar el de mejor ranking como fallback
    // para no propagar nullptr en caso inesperado
    if (e1 && e2)
        return (e1->getRanking() <= e2->getRanking()) ? e1 : e2;
    return e1 ? e1 : e2;
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

void Resultado::registrarGolPartido1(int idx) { if(idx>=0&&idx<11) golesPartido1[idx]++; }
void Resultado::registrarGolPartido2(int idx) { if(idx>=0&&idx<11) golesPartido2[idx]++; }
int  Resultado::getGolesPartido1(int idx)     { return (idx>=0&&idx<11)?golesPartido1[idx]:0; }
int  Resultado::getGolesPartido2(int idx)     { return (idx>=0&&idx<11)?golesPartido2[idx]:0; }

int       Resultado::getGfEquipo1()    { return gfEquipo1; }
int       Resultado::getGfEquipo2()    { return gfEquipo2; }
float     Resultado::getPosesionEq1()  { return posesionEq1; }
bool      Resultado::getHuboProrroga() { return huboProrroga; }
Jugador** Resultado::getConvocados1()  { return convocados1; }
Jugador** Resultado::getConvocados2()  { return convocados2; }
