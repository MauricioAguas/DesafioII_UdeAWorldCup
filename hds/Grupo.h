#ifndef GRUPO_H
#define GRUPO_H

#include "Equipo.h"
#include "Partido.h"
#include <iostream>
using namespace std;

class Grupo {
private:
    char letra;
    int cantEquipos;
    Equipo* equipos[4];
    Partido* partidos[6];
    int puntos[4];
    int difGoles[4];
    int golesFavor[4];

public:
    Grupo();
    Grupo(char letra);
    ~Grupo();

    void agregarEquipo(Equipo* e);
    void configurarPartidos(string fechaBase);
    void simularPartidos();
    void generarTablaClasif();
    void imprimirTabla();
    Equipo** obtenerClasificados();
    char getLetra();
    Equipo* getEquipo(int i);
    int getCantEquipos();
    int getPuntos(int i);
};

#endif
