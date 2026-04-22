#include "EstadisticasEquipo.h"

EstadisticasEquipo::EstadisticasEquipo() {
    golesFavor  = 0;
    golesContra = 0;
    ganados     = 0;
    empatados   = 0;
    perdidos    = 0;
    amarillas   = 0;
    rojas       = 0;
    faltas      = 0;
}

EstadisticasEquipo::~EstadisticasEquipo() {}

// resultado: 1 = ganó, 0 = empató, -1 = perdió
void EstadisticasEquipo::actualizar(int gf, int gc, int resultado) {
    golesFavor  += gf;
    golesContra += gc;
    if (resultado == 1)       ganados++;
    else if (resultado == 0)  empatados++;
    else                      perdidos++;
}

int EstadisticasEquipo::getDiferenciaGoles() {
    return golesFavor - golesContra;
}

int EstadisticasEquipo::getGolesFavor() {
    return golesFavor;
}

int EstadisticasEquipo::getGolesContra() {
    return golesContra;
}

EstadisticasEquipo EstadisticasEquipo::operator+(const EstadisticasEquipo& otro) const {
    EstadisticasEquipo resultado;
    resultado.golesFavor  = golesFavor  + otro.golesFavor;
    resultado.golesContra = golesContra + otro.golesContra;
    resultado.ganados     = ganados     + otro.ganados;
    resultado.empatados   = empatados   + otro.empatados;
    resultado.perdidos    = perdidos    + otro.perdidos;
    resultado.amarillas   = amarillas   + otro.amarillas;
    resultado.rojas       = rojas       + otro.rojas;
    resultado.faltas      = faltas      + otro.faltas;
    return resultado;
}
