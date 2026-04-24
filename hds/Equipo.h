#ifndef EQUIPO_H
#define EQUIPO_H

#include <string>
#include "Jugador.h"
#include "EstadisticasEquipo.h"
using namespace std;

class Equipo {
private:
    string pais;
    string confederacion;
    int ranking;
    string DT;
    Jugador** plantilla;
    EstadisticasEquipo* estadisticas;

public:
    Equipo();
    Equipo(string pais, string confederacion, int ranking, string DT);
    ~Equipo();

    void cargarJugadores();
    void setEstadisticasIniciales(int gf, int gc, int gan, int emp, int per);
    Jugador** seleccionarConvocados();
    void actualizarEstadisticas(int gf, int gc, int resultado);

    int    getGFA();
    int    getGEC();
    int    getPartidosJugados();
    string getPais();
    int    getRanking();
    string getConfederacion();
    EstadisticasEquipo* getEstadisticas();

    // Getter para acceder a jugador individual de la plantilla (0..25)
    Jugador* getJugador(int i);
    int      getTamanoPlantilla();

    friend ostream& operator<<(ostream& os, const Equipo& e);
};

#endif
