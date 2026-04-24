#ifndef ESTADISTICASJUGADOR_H
#define ESTADISTICASJUGADOR_H

class EstadisticasJugador {
private:
    int partidosJugados;
    int goles;
    int minutosJugados;
    int asistencias;
    int amarillas;
    int rojas;
    int faltas;

public:
    EstadisticasJugador();
    ~EstadisticasJugador();

    void actualizar(int goles, int minutos, int amarillas, int rojas, int faltas);
    int getGoles();
    int getMinutos();

    EstadisticasJugador operator+(const EstadisticasJugador& otro) const;
};

#endif
