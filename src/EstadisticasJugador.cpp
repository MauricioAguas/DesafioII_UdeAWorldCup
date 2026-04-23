#include "../include/EstadisticasJugador.h"

EstadisticasJugador::EstadisticasJugador() {
    partidosJugados = goles = minutosJugados = asistencias = 0;
    amarillas = rojas = faltas = 0;
}

EstadisticasJugador::~EstadisticasJugador() {}

void EstadisticasJugador::actualizar(int g, int min, int am, int ro, int fa) {
    goles          += g;
    minutosJugados += min;
    amarillas      += am;
    rojas          += ro;
    faltas         += fa;
    if (min > 0) partidosJugados++;
}

int EstadisticasJugador::getGoles()   { return goles; }
int EstadisticasJugador::getMinutos() { return minutosJugados; }

EstadisticasJugador EstadisticasJugador::operator+(const EstadisticasJugador& o) const {
    EstadisticasJugador r;
    r.goles          = goles          + o.goles;
    r.minutosJugados = minutosJugados + o.minutosJugados;
    r.asistencias    = asistencias    + o.asistencias;
    r.amarillas      = amarillas      + o.amarillas;
    r.rojas          = rojas          + o.rojas;
    r.faltas         = faltas         + o.faltas;
    r.partidosJugados= partidosJugados+ o.partidosJugados;
    return r;
}
