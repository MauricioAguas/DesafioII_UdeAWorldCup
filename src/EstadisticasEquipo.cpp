#include "../include/EstadisticasEquipo.h"

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

void EstadisticasEquipo::actualizar(int gf, int gc, int resultado) {
    golesFavor  += gf;
    golesContra += gc;
    if      (resultado > 0) ganados++;
    else if (resultado == 0) empatados++;
    else                     perdidos++;
}

int EstadisticasEquipo::getGolesFavor()  { return golesFavor;  }
int EstadisticasEquipo::getGolesContra() { return golesContra; }
int EstadisticasEquipo::getGanados()     { return ganados;     }
int EstadisticasEquipo::getEmpatados()   { return empatados;   }
int EstadisticasEquipo::getPerdidos()    { return perdidos;    }
int EstadisticasEquipo::getAmarillas()   { return amarillas;   }
int EstadisticasEquipo::getRojas()       { return rojas;       }
int EstadisticasEquipo::getFaltas()      { return faltas;      }

void EstadisticasEquipo::setGolesFavor(int v)  { golesFavor  = v; }
void EstadisticasEquipo::setGolesContra(int v) { golesContra = v; }
void EstadisticasEquipo::setGanados(int v)     { ganados     = v; }
void EstadisticasEquipo::setEmpatados(int v)   { empatados   = v; }
void EstadisticasEquipo::setPerdidos(int v)    { perdidos    = v; }
void EstadisticasEquipo::setAmarillas(int v)   { amarillas   = v; }
void EstadisticasEquipo::setRojas(int v)       { rojas       = v; }
void EstadisticasEquipo::setFaltas(int v)      { faltas      = v; }
