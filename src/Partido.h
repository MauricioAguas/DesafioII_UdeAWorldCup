#ifndef PARTIDO_H
#define PARTIDO_H

#include <string>
#include "Equipo.h"
#include "Resultado.h"
using namespace std;

class Partido {
private:
    string fecha;
    string hora;
    string sede;
    string codsArbitros[3];
    Equipo* equipo1;
    Equipo* equipo2;
    Resultado* resultado;

public:
    Partido();
    Partido(string fecha, string hora, string sede, Equipo* e1, Equipo* e2);
    ~Partido();

    void simular();
    float calcularGolesEsperados(Equipo* ataque, Equipo* defensa);
    void asignarGoleadores();
    void imprimirResumen();

    friend ostream& operator<<(ostream& os, const Partido& p);
};

#endif
