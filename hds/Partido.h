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
    void simularConProrroga();
    float calcularGolesEsperados(Equipo* ataque, Equipo* defensa);
    void asignarGoleadores();
    void imprimirResumen();

    Resultado* getResultado() const;
    Equipo*    getEquipo1();
    Equipo*    getEquipo2();
    string     getFecha();

    friend ostream& operator<<(ostream& os, const Partido& p);
};

#endif
