#ifndef MUNDIAL_H
#define MUNDIAL_H

#include "Fase.h"

class Mundial {
private:
    int anio;
    int cantEquipos;

public:
    Mundial();
    Mundial(int anio, int cantEquipos);
    ~Mundial();

    void cargarEquipos();
    void conformarGrupos();
    void simularTorneo();
    void generarEstadisticas();
};

#endif
