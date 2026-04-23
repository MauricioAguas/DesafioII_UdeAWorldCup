#include "../include/Partido.h"
#include <iostream>
#include <cstdlib>
using namespace std;

Partido::Partido() {
    fecha = hora = sede = "";
    equipo1 = equipo2 = nullptr;
    resultado = nullptr;
    codsArbitros[0] = "codArbitro1";
    codsArbitros[1] = "codArbitro2";
    codsArbitros[2] = "codArbitro3";
}

Partido::Partido(string fecha, string hora, string sede, Equipo* e1, Equipo* e2) {
    this->fecha = fecha; this->hora = hora; this->sede = sede;
    this->equipo1 = e1; this->equipo2 = e2;
    resultado = nullptr;
    codsArbitros[0] = "codArbitro1";
    codsArbitros[1] = "codArbitro2";
    codsArbitros[2] = "codArbitro3";
}

Partido::~Partido() { if (resultado) delete resultado; }

float Partido::calcularGolesEsperados(Equipo* ataque, Equipo* defensa) {
    float promGFA = (ataque->getGFA()  > 0) ? (float)ataque->getGFA()  : 1.0f;
    float promGEC = (defensa->getGEC() > 0) ? (float)defensa->getGEC() : 1.0f;
    return 0.4f * promGFA + 0.6f * promGEC + 1.35f;
}

static void distribuirGoles(Jugador** conv, int total) {
    int asignados = 0;
    for (int p = 0; p < 3 && asignados < total; p++)
        for (int i = 0; i < 11 && asignados < total; i++)
            if ((rand() % 100) < 4) { conv[i]->actualizarEstadisticas(1,0,0,0,0); asignados++; }
    while (asignados < total) { conv[0]->actualizarEstadisticas(1,0,0,0,0); asignados++; }
}

static void asignarTarjetasYFaltas(Jugador** conv) {
    for (int i = 0; i < 11; i++) {
        int f = 0, am = 0;
        if ((rand()%10000)<1300){ f++; if((rand()%10000)<275){ f++; if((rand()%10000)<70) f++; } }
        if ((rand()%10000)<600) { am++; if((rand()%10000)<115) am++; }
        int ro = (am >= 2) ? 1 : 0;
        if (f>0||am>0) conv[i]->actualizarEstadisticas(0,0,am,ro,f);
    }
}

void Partido::simular() {
    Jugador** c1 = equipo1->seleccionarConvocados();
    Jugador** c2 = equipo2->seleccionarConvocados();
    int gf1 = (int)(calcularGolesEsperados(equipo1,equipo2) * ((rand()%200)/100.0f));
    int gf2 = (int)(calcularGolesEsperados(equipo2,equipo1) * ((rand()%200)/100.0f));
    resultado = new Resultado(gf1, gf2, false, c1, c2);
    resultado->calcularPosesion(equipo1->getRanking(), equipo2->getRanking());
    asignarGoleadores();
    asignarTarjetasYFaltas(c1);
    asignarTarjetasYFaltas(c2);
    resultado->actualizarHistoricos(equipo1, equipo2);
    delete[] c1; delete[] c2;
}

void Partido::simularConProrroga() {
    simular();
    if (resultado->getGfEquipo1() == resultado->getGfEquipo2()) {
        int r1 = equipo1->getRanking(), r2 = equipo2->getRanking();
        int umbral = (r1+r2 > 0) ? (r2*100/(r1+r2)) : 50;
        int gf1 = resultado->getGfEquipo1();
        int gf2 = resultado->getGfEquipo2();
        delete resultado; resultado = nullptr;
        Jugador** c1 = equipo1->seleccionarConvocados();
        Jugador** c2 = equipo2->seleccionarConvocados();
        if ((rand()%100) < umbral) gf1++; else gf2++;
        resultado = new Resultado(gf1, gf2, true, c1, c2);
        resultado->calcularPosesion(r1, r2);
        resultado->actualizarHistoricos(equipo1, equipo2);
        delete[] c1; delete[] c2;
    }
}

void Partido::asignarGoleadores() {
    if (!resultado) return;
    distribuirGoles(resultado->getConvocados1(), resultado->getGfEquipo1());
    distribuirGoles(resultado->getConvocados2(), resultado->getGfEquipo2());
}

void Partido::imprimirResumen() {
    cout << "\n=== Partido ===\nFecha: " << fecha << "  Hora: " << hora << "  Sede: " << sede;
    cout << "\n" << equipo1->getPais() << " " << resultado->getGfEquipo1()
         << " - " << resultado->getGfEquipo2() << " " << equipo2->getPais();
    if (resultado->getHuboProrroga()) cout << " (Prorroga)";
    cout << "\nGoleadores " << equipo1->getPais() << ": ";
    for (int i=0;i<11;i++) { Jugador* j=resultado->getConvocados1()[i]; if(j&&j->getGoles()>0) cout<<j->getNumeroCamiseta()<<" "; }
    cout << "\nGoleadores " << equipo2->getPais() << ": ";
    for (int i=0;i<11;i++) { Jugador* j=resultado->getConvocados2()[i]; if(j&&j->getGoles()>0) cout<<j->getNumeroCamiseta()<<" "; }
    cout << endl;
}

Resultado* Partido::getResultado() { return resultado; }
Equipo*    Partido::getEquipo1()   { return equipo1; }
Equipo*    Partido::getEquipo2()   { return equipo2; }
string     Partido::getFecha()     { return fecha; }

ostream& operator<<(ostream& os, const Partido& p) {
    os << p.fecha << " " << p.hora << " | " << p.equipo1->getPais() << " vs " << p.equipo2->getPais() << " | Sede: " << p.sede;
    return os;
}
