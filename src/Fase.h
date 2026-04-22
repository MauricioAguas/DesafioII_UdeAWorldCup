#ifndef FASE_H
#define FASE_H

#include <string>
using namespace std;

class Fase {
private:
    string nombre;
    string tipo;
    string fechaInicio;

public:
    Fase();
    Fase(string nombre, string tipo, string fechaInicio);
    ~Fase();

    void configurarPartidos();
    void simularPartidos();
    void imprimirResultados();
};

#endif
