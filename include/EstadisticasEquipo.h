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

    void actualizar(int gf, int gc, int resultado);

    int getGolesFavor();
    int getGolesContra();
    int getGanados();
    int getEmpatados();
    int getPerdidos();
    int getAmarillas();
    int getRojas();
    int getFaltas();

    // Setters para carga inicial desde CSV
    void setGolesFavor(int v);
    void setGolesContra(int v);
    void setGanados(int v);
    void setEmpatados(int v);
    void setPerdidos(int v);
    void setAmarillas(int v);
    void setRojas(int v);
    void setFaltas(int v);
};

#endif
