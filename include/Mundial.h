#ifndef MUNDIAL_H
#define MUNDIAL_H

#include <string>
#include "Fase.h"
#include "Equipo.h"
using namespace std;

class Mundial {
private:
    int    anio;
    int    cantEquipos;
    Equipo* equipos[48];
    Grupo*  grupos[12];
    Fase*   fases[8];
    int     cantFases;

public:
    Mundial();
    Mundial(int anio, int cantEquipos);
    ~Mundial();

    void cargarEquipos();
    void conformarGrupos();
    void simularTorneo();
    void generarEstadisticas();

    void armarR16(Fase* faseGrupos, Fase* faseR16);
    Equipo* getEquipoMasGoles();
    Equipo* getCampeon();
};

#endif
