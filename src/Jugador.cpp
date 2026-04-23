#include "../include/Jugador.h"
#include <iostream>
using namespace std;

Jugador::Jugador() {
    nombre = apellido = "";
    numeroCamiseta = 0;
    estadisticas = new EstadisticasJugador();
}

Jugador::Jugador(string nombre, string apellido, int numeroCamiseta) {
    this->nombre         = nombre;
    this->apellido       = apellido;
    this->numeroCamiseta = numeroCamiseta;
    estadisticas = new EstadisticasJugador();
}

Jugador::~Jugador() { delete estadisticas; }

void Jugador::actualizarEstadisticas(int goles, int minutos, int amarillas, int rojas, int faltas) {
    estadisticas->actualizar(goles, minutos, amarillas, rojas, faltas);
}

int    Jugador::getGoles()         { return estadisticas->getGoles(); }
int    Jugador::getNumeroCamiseta() { return numeroCamiseta; }
string Jugador::getNombre()        { return nombre + " " + apellido; }

ostream& operator<<(ostream& os, const Jugador& j) {
    os << j.apellido << ", " << j.nombre
       << " | Camiseta: " << j.numeroCamiseta
       << " | Goles: "    << j.estadisticas->getGoles()
       << " | Minutos: "  << j.estadisticas->getMinutos();
    return os;
}
