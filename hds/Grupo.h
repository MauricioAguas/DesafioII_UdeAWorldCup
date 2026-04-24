#ifndef GRUPO_H
#define GRUPO_H

#include <string>
#include "Partido.h"
using namespace std;

class Grupo {
private:
    char  letra;
    Equipo* equipos[4];
    int   cantEquipos;
    Partido* partidos[6];
    int   puntos[4];
    int   difGoles[4];
    int   golesFavor[4];

public:
    Grupo();
    Grupo(char letra);
    ~Grupo();

    void agregarEquipo(Equipo* e);
    void configurarPartidos(string fechaBase);
    void simularPartidos();
    void generarTablaClasif();
    void imprimirTabla();
    void imprimirPartidos();

    Equipo** obtenerClasificados();
    char     getLetra();
    Equipo*  getEquipo(int i);
    int      getCantEquipos();
    int      getPuntos(int i);
    int      getDifGoles(int i);
    int      getGolesFavor(int i);
};

#endif
