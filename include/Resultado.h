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
    Resultado(int gf1, int gf2, bool prorroga, Jugador** conv1, Jugador** conv2);
    ~Resultado();

    void calcularPosesion(int rankA, int rankB);
    Equipo* getGanador(Equipo* e1, Equipo* e2);
    void actualizarHistoricos(Equipo* e1, Equipo* e2);
    int getGfEquipo1();
    int getGfEquipo2();
    bool getHuboProrroga();
    Jugador** getConvocados1();
    Jugador** getConvocados2();
};

#endif
