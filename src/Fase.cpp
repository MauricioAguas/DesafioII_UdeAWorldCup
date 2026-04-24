#include "../hds/Fase.h"
#include <iostream>
using namespace std;

Fase::Fase() {
    nombre=tipo=fechaInicio=""; cantGrupos=cantPartidos=0;
    for(int i=0;i<12;i++) grupos[i]=nullptr;
    for(int i=0;i<32;i++) partidos[i]=nullptr;
}

Fase::Fase(string nombre, string tipo, string fechaInicio) : Fase() {
    this->nombre=nombre; this->tipo=tipo; this->fechaInicio=fechaInicio;
}

Fase::~Fase() {
    if (tipo=="eliminacion") {
        for(int i=0;i<cantPartidos;i++) {
            if(partidos[i]) delete partidos[i];
        }
    }
}

void Fase::agregarGrupo(Grupo* g)     { if(cantGrupos<12)   grupos[cantGrupos++]=g; }
void Fase::agregarPartido(Partido* p) { if(cantPartidos<32) partidos[cantPartidos++]=p; }

void Fase::configurarPartidos() {
    for(int i=0;i<cantGrupos;i++) {
        if(grupos[i]) grupos[i]->configurarPartidos(fechaInicio);
    }
}

void Fase::simularPartidos() {
    if (tipo=="grupos") {
        for(int i=0;i<cantGrupos;i++) {
            if(grupos[i]) grupos[i]->simularPartidos();
        }
    } else {
        simularEliminatoria();
    }
}

void Fase::generarTablas() {
    for(int i=0;i<cantGrupos;i++) {
        if(grupos[i]) grupos[i]->generarTablaClasif();
    }
}

void Fase::imprimirResultados() {
    cout << "\n========== " << nombre << " ==========\n";
    if (tipo=="grupos") {
        // Imprimir todos los partidos de cada grupo
        for(int i=0;i<cantGrupos;i++) {
            if(grupos[i]) grupos[i]->imprimirPartidos();
        }
        // Imprimir tablas de clasificacion
        for(int i=0;i<cantGrupos;i++) {
            if(grupos[i]) grupos[i]->imprimirTabla();
        }
    } else {
        for(int i=0;i<cantPartidos;i++) {
            if(partidos[i]) partidos[i]->imprimirResumen();
        }
    }
}

void Fase::simularEliminatoria() {
    for(int i=0;i<cantPartidos;i++) {
        if(partidos[i]) partidos[i]->simularConProrroga();
    }
}

Equipo* Fase::getGanadorPartido(int i) {
    if(i<0||i>=cantPartidos||!partidos[i]) return nullptr;
    Resultado* res=partidos[i]->getResultado();
    if(!res) return nullptr;
    return res->getGanador(partidos[i]->getEquipo1(), partidos[i]->getEquipo2());
}

string   Fase::getNombre()       { return nombre; }
int      Fase::getCantGrupos()   { return cantGrupos; }
Grupo*   Fase::getGrupo(int i)   { return (i>=0&&i<cantGrupos)?grupos[i]:nullptr; }
int      Fase::getCantPartidos() { return cantPartidos; }
Partido* Fase::getPartido(int i) { return (i>=0&&i<cantPartidos)?partidos[i]:nullptr; }
