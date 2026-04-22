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
    Jugador** seleccionarConvocados();
    void actualizarEstadisticas();
    int getGFA();
    int getGEC();
    string getPais();
    int getRanking();
    string getConfederacion();

    friend ostream& operator<<(ostream& os, const Equipo& e);
};

#endif
