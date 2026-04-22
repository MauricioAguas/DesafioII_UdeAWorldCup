#ifndef RESULTADO_H
#define RESULTADO_H

#include "Jugador.h"
#include "Equipo.h"

class Resultado {
private:
    int gfEquipo1;
    int gfEquipo2;
    float posesionEq1;
    bool huboProrroga;
    Jugador* convocados1[11];
    Jugador* convocados2[11];

public:
    Resultado();
    ~Resultado();

    void calcularPosesion(int rankA, int rankB);
    Equipo* getGanador(Equipo* e1, Equipo* e2);
    void actualizarHistoricos();
    int getGfEquipo1();
    int getGfEquipo2();
};

#endif
