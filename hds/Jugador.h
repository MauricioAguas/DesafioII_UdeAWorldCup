#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <iostream>
#include "EstadisticasJugador.h"
using namespace std;

class Jugador {
private:
    string nombre;
    string apellido;
    int numeroCamiseta;
    EstadisticasJugador* estadisticas;

public:
    Jugador();
    Jugador(string nombre, string apellido, int numeroCamiseta, int golesIniciales = 0);
    ~Jugador();

    string getNombre();
    string getApellido();
    int    getNumeroCamiseta();
    EstadisticasJugador* getEstadisticas();

    // Delegadores usados en Partido.cpp y Resultado.cpp
    void actualizarEstadisticas(int goles, int minutos, int amarillas, int rojas, int faltas);
    int  getGoles();

    friend ostream& operator<<(ostream& os, const Jugador& j);
};

#endif
