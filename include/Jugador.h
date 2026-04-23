#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
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
    Jugador(string nombre, string apellido, int numeroCamiseta);
    ~Jugador();

    void actualizarEstadisticas(int goles, int minutos, int amarillas, int rojas, int faltas);
    int getGoles();
    int getNumeroCamiseta();
    string getNombre();

    friend ostream& operator<<(ostream& os, const Jugador& j);
};

#endif
