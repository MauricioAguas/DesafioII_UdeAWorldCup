#ifndef FASE_H
#define FASE_H

#include <string>
#include "Grupo.h"
#include "Partido.h"
using namespace std;

class Fase {
private:
    string nombre;
    string tipo;
    string fechaInicio;
    Grupo*   grupos[12];
    int      cantGrupos;
    Partido* partidos[32];
    int      cantPartidos;

public:
    Fase();
    Fase(string nombre, string tipo, string fechaInicio);
    ~Fase();

    void agregarGrupo(Grupo* g);
    void configurarPartidos();
    void simularPartidos();
    void generarTablas();
    void imprimirResultados();

    void agregarPartido(Partido* p);
    void simularEliminatoria();
    Equipo* getGanadorPartido(int i);

    string   getNombre();
    int      getCantGrupos();
    Grupo*   getGrupo(int i);
    int      getCantPartidos() const;
    Partido* getPartido(int i) const;
};

#endif
