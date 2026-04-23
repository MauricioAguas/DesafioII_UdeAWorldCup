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
    void simularConProrroga(); // empate en eliminatorias -> prorroga sesgada por ranking
    float calcularGolesEsperados(Equipo* ataque, Equipo* defensa);
    void asignarGoleadores();
    void imprimirResumen();

    // Getters necesarios para Grupo y Fase
    Resultado* getResultado();
    Equipo*    getEquipo1();
    Equipo*    getEquipo2();
    string     getFecha();

    friend ostream& operator<<(ostream& os, const Partido& p);
};

#endif
