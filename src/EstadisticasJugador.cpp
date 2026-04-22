#include "EstadisticasJugador.h"

EstadisticasJugador::EstadisticasJugador() {
    partidosJugados = 0;
    goles = 0;
    minutosJugados = 0;
    asistencias = 0;
    amarillas = 0;
    rojas = 0;
    faltas = 0;
}

EstadisticasJugador::~EstadisticasJugador() {}

void EstadisticasJugador::actualizar(int g, int minutos, int am, int ro, int fa) {
    partidosJugados++;
    goles += g;
    minutosJugados += minutos;
    amarillas += am;
    rojas += ro;
    faltas += fa;
}

int EstadisticasJugador::getGoles() {
    return goles;
}

int EstadisticasJugador::getMinutos() {
    return minutosJugados;
}

EstadisticasJugador EstadisticasJugador::operator+(const EstadisticasJugador& otro) const {
    EstadisticasJugador resultado;
    resultado.partidosJugados = partidosJugados + otro.partidosJugados;
    resultado.goles           = goles + otro.goles;
    resultado.minutosJugados  = minutosJugados + otro.minutosJugados;
    resultado.asistencias     = asistencias + otro.asistencias;
    resultado.amarillas       = amarillas + otro.amarillas;
    resultado.rojas           = rojas + otro.rojas;
    resultado.faltas          = faltas + otro.faltas;
    return resultado;
}
