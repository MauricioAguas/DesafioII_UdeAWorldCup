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

    // --- Req V: medicion de recursos ---
    long long iteraciones;
    long long calcularMemoria() const;

public:
    Mundial();
    Mundial(int anio, int cantEquipos);
    ~Mundial();

    void cargarEquipos();
    void conformarGrupos();
    void simularTorneo();
    void generarEstadisticas();

    void    armarR16(Fase* faseGrupos, Fase* faseR16);
    Equipo* getEquipoMasGoles();
    Equipo* getCampeon();

    void imprimirMetricas(const string& nombreFunc) const;

    // --- Getters para menu interactivo ---
    int     getCantEquipos()  const { return cantEquipos; }
    int     getCantFases()    const { return cantFases; }
    int     getCantGrupos()   const { return 12; }
    Equipo* getEquipo(int i)  const { return (i >= 0 && i < cantEquipos) ? equipos[i] : nullptr; }
    Fase*   getFase(int i)    const { return (i >= 0 && i < cantFases)   ? fases[i]   : nullptr; }
    Grupo*  getGrupo(int i)   const { return (i >= 0 && i < 12)          ? grupos[i]  : nullptr; }
};

#endif
