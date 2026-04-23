#include "../include/Jugador.h"
#include <iostream>
using namespace std;

Jugador::Jugador() {
    nombre         = "";
    apellido       = "";
    numeroCamiseta = 0;
    estadisticas   = new EstadisticasJugador();
}

// Req I: constructor con goles historicos iniciales
// Los goles se cargan via actualizar(); el resto de stats inician en 0
Jugador::Jugador(string nombre, string apellido, int numeroCamiseta, int golesIniciales) {
    this->nombre         = nombre;
    this->apellido       = apellido;
    this->numeroCamiseta = numeroCamiseta;
    estadisticas         = new EstadisticasJugador();
    if (golesIniciales > 0)
        estadisticas->actualizar(golesIniciales, 0, 0, 0, 0);
}

Jugador::~Jugador() {
    delete estadisticas;
}

string Jugador::getNombre()         { return nombre;         }
string Jugador::getApellido()       { return apellido;       }
int    Jugador::getNumeroCamiseta() { return numeroCamiseta; }
EstadisticasJugador* Jugador::getEstadisticas() { return estadisticas; }

ostream& operator<<(ostream& os, const Jugador& j) {
    os << j.nombre << " " << j.apellido << " #" << j.numeroCamiseta;
    return os;
}
