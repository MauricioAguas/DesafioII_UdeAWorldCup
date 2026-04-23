#ifndef FASE_H
#define FASE_H

#include <string>
#include "Grupo.h"
#include "Partido.h"
using namespace std;

// tipo: "grupos" o "eliminacion"
class Fase {
private:
    string nombre;
    string tipo;
    string fechaInicio;

    // Para fase de grupos
    Grupo*   grupos[12];
    int      cantGrupos;

    // Para fases eliminatorias (R16=32, R8=16, QF=8, SF=4, 3er=1, Final=1)
    Partido* partidos[32];
    int      cantPartidos;

public:
    Fase();
    Fase(string nombre, string tipo, string fechaInicio);
    ~Fase();

    // Fase de grupos
    void agregarGrupo(Grupo* g);
    void configurarPartidos();          // delega a cada Grupo
    void simularPartidos();             // delega a cada Grupo
    void generarTablas();               // genera tabla de clasificacion de cada grupo
    void imprimirResultados();

    // Fase eliminatoria
    void agregarPartido(Partido* p);
    void simularEliminatoria();         // simula con prorroga si empate
    Equipo* getGanadorPartido(int i);   // retorna ganador del partido i

    // Getters
    string   getNombre();
    int      getCantGrupos();
    Grupo*   getGrupo(int i);
    int      getCantPartidos();
    Partido* getPartido(int i);
};

#endif
