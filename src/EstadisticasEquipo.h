#ifndef ESTADISTICASEQUIPO_H
#define ESTADISTICASEQUIPO_H

class EstadisticasEquipo {
private:
    int golesFavor;
    int golesContra;
    int ganados;
    int empatados;
    int perdidos;
    int amarillas;
    int rojas;
    int faltas;

public:
    EstadisticasEquipo();
    ~EstadisticasEquipo();

    void actualizar(int gf, int gc, int resultado);
    int getDiferenciaGoles();
    int getGolesFavor();
    int getGolesContra();

    EstadisticasEquipo operator+(const EstadisticasEquipo& otro) const;
};

#endif
