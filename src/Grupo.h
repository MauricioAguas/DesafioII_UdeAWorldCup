#ifndef GRUPO_H
#define GRUPO_H

#include "Equipo.h"
#include "Partido.h"
#include <iostream>
using namespace std;

class Grupo {
private:
    char letra;
    int cantEquipos;     // max 4
    Equipo* equipos[4];
    Partido* partidos[6]; // round-robin: C(4,2) = 6 partidos
    int puntos[4];        // puntos acumulados por equipo
    int difGoles[4];      // diferencia de goles por equipo
    int golesFavor[4];    // goles a favor acumulados

public:
    Grupo();
    Grupo(char letra);
    ~Grupo();

    void agregarEquipo(Equipo* e);
    void configurarPartidos(string fechaBase); // genera los 6 partidos con fechas
    void simularPartidos();                    // simula los 6 partidos
    void generarTablaClasif();                 // calcula puntos y ordena
    void imprimirTabla();
    Equipo** obtenerClasificados();            // retorna arreglo de 4 equipos ordenados
    char getLetra();
    Equipo* getEquipo(int i);
    int getCantEquipos();
    int getPuntos(int i);
};

#endif
