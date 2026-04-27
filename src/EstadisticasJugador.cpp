#include "../hds/EstadisticasJugador.h"

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

int EstadisticasJugador::getGoles()          { return goles; }
int EstadisticasJugador::getMinutos()        { return minutosJugados; }
int EstadisticasJugador::getPartidosJugados(){ return partidosJugados; }
int EstadisticasJugador::getAmarillas()      { return amarillas; }
int EstadisticasJugador::getRojas()          { return rojas; }
int EstadisticasJugador::getFaltas()         { return faltas; }
int EstadisticasJugador::getAsistencias()    { return asistencias; }

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
