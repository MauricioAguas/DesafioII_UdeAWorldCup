#include "../include/EstadisticasEquipo.h"

EstadisticasEquipo::EstadisticasEquipo() {
    golesFavor = golesContra = ganados = empatados = perdidos = 0;
    amarillas  = rojas = faltas = 0;
}

EstadisticasEquipo::~EstadisticasEquipo() {}

void EstadisticasEquipo::actualizar(int gf, int gc, int resultado) {
    golesFavor  += gf;
    golesContra += gc;
    if      (resultado ==  1) ganados++;
    else if (resultado ==  0) empatados++;
    else                      perdidos++;
}

int EstadisticasEquipo::getDiferenciaGoles() { return golesFavor - golesContra; }
int EstadisticasEquipo::getGolesFavor()      { return golesFavor; }
int EstadisticasEquipo::getGolesContra()     { return golesContra; }

EstadisticasEquipo EstadisticasEquipo::operator+(const EstadisticasEquipo& o) const {
    EstadisticasEquipo r;
    r.golesFavor  = golesFavor  + o.golesFavor;
    r.golesContra = golesContra + o.golesContra;
    r.ganados     = ganados     + o.ganados;
    r.empatados   = empatados   + o.empatados;
    r.perdidos    = perdidos    + o.perdidos;
    r.amarillas   = amarillas   + o.amarillas;
    r.rojas       = rojas       + o.rojas;
    r.faltas      = faltas      + o.faltas;
    return r;
}
