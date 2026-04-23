#ifndef MUNDIAL_H
#define MUNDIAL_H

#include <string>
#include "Fase.h"
#include "Equipo.h"
using namespace std;

class Mundial {
private:
    int    anio;
    int    cantEquipos;   // 48
    Equipo* equipos[48];
    Grupo*  grupos[12];
    Fase*   fases[8];     // grupos, R16, R8, QF, SF, 3er, Final
    int     cantFases;

public:
    Mundial();
    Mundial(int anio, int cantEquipos);
    ~Mundial();

    void cargarEquipos();       // lee CSV y construye los 48 Equipo*
    void conformarGrupos();     // sorteo con restricciones de confederacion
    void simularTorneo();       // ejecuta todas las fases
    void generarEstadisticas(); // imprime estadisticas finales

    // Helpers internos
    void armarR16(Fase* faseGrupos, Fase* faseR16);
    Equipo* getEquipoMasGoles();
    Equipo* getCampeon();
};

#endif
