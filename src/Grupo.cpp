#include "../hds/Grupo.h"
#include <iostream>
using namespace std;

Grupo::Grupo() {
    letra = ' '; cantEquipos = 0;
    for (int i=0;i<4;i++) { equipos[i]=nullptr; puntos[i]=difGoles[i]=golesFavor[i]=0; }
    for (int i=0;i<6;i++) partidos[i]=nullptr;
}

Grupo::Grupo(char letra) : Grupo() { this->letra = letra; }

Grupo::~Grupo() {
    for (int i=0;i<6;i++) if (partidos[i]) delete partidos[i];
}

void Grupo::agregarEquipo(Equipo* e) { if (cantEquipos<4) equipos[cantEquipos++]=e; }

// Metodo legacy
void Grupo::configurarPartidos(string fechaBase) {
    int e1idx[6]={0,2,0,1,0,1}, e2idx[6]={1,3,2,3,3,2};
    int offset = (letra-'A') % 4;
    int diasBase[3]={0+offset, 4+offset, 8+offset};
    int anio=stoi(fechaBase.substr(0,4)), mes=stoi(fechaBase.substr(5,2)), dia=stoi(fechaBase.substr(8,2));
    int diasMes[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    for (int p=0;p<6;p++) {
        int gf=p/2, dp=dia+diasBase[gf], mp=mes, ap=anio;
        while (dp>diasMes[mp]) { dp-=diasMes[mp]; mp++; if(mp>12){mp=1;ap++;} }
        string f=to_string(ap)+"-"+(mp<10?"0":"")+to_string(mp)+"-"+(dp<10?"0":"")+to_string(dp);
        Equipo* ea = equipos[e1idx[p]];
        Equipo* eb = equipos[e2idx[p]];
        if (ea && eb && ea != eb)
            partidos[p]=new Partido(f,"00:00","nombreSede",ea,eb);
        else
            partidos[p]=nullptr;
    }
}

// Nuevo metodo: recibe las 6 fechas ya calculadas por el scheduler global de Mundial
void Grupo::configurarPartidosConFechas(string fechas[6]) {
    int e1idx[6]={0,0,0,1,1,2};
    int e2idx[6]={1,2,3,2,3,3};
    for (int p=0;p<6;p++) {
        if (partidos[p]) { delete partidos[p]; partidos[p]=nullptr; }
        Equipo* ea = (e1idx[p] < cantEquipos) ? equipos[e1idx[p]] : nullptr;
        Equipo* eb = (e2idx[p] < cantEquipos) ? equipos[e2idx[p]] : nullptr;
        if (ea && eb && ea != eb)
            partidos[p] = new Partido(fechas[p], "00:00", "nombreSede", ea, eb);
        else
            partidos[p] = nullptr;
    }
}

void Grupo::simularPartidos() {
    for (int i=0;i<6;i++) {
        if (!partidos[i]) continue;
        if (partidos[i]->getEquipo1() == partidos[i]->getEquipo2()) continue;
        partidos[i]->simular();
    }
}

void Grupo::generarTablaClasif() {
    for (int i=0;i<4;i++) puntos[i]=difGoles[i]=golesFavor[i]=0;
    for (int p=0;p<6;p++) {
        if (!partidos[p]) continue;
        Resultado* res=partidos[p]->getResultado();
        if (!res) continue;
        int gf1=res->getGfEquipo1(), gf2=res->getGfEquipo2();
        Equipo* eq1=partidos[p]->getEquipo1(), *eq2=partidos[p]->getEquipo2();
        if (!eq1 || !eq2 || eq1==eq2) continue;
        int i1=-1, i2=-1;
        for (int i=0;i<4;i++) { if(equipos[i]==eq1) i1=i; if(equipos[i]==eq2) i2=i; }
        if (i1<0||i2<0) continue;
        golesFavor[i1]+=gf1; golesFavor[i2]+=gf2;
        difGoles[i1]+=gf1-gf2; difGoles[i2]+=gf2-gf1;
        if (gf1>gf2) puntos[i1]+=3;
        else if (gf1<gf2) puntos[i2]+=3;
        else { puntos[i1]++; puntos[i2]++; }
    }
    for (int i=0;i<3;i++) for (int j=0;j<3-i;j++) {
        bool sw=false;
        if (puntos[j]<puntos[j+1]) sw=true;
        else if (puntos[j]==puntos[j+1]&&difGoles[j]<difGoles[j+1]) sw=true;
        else if (puntos[j]==puntos[j+1]&&difGoles[j]==difGoles[j+1]&&golesFavor[j]<golesFavor[j+1]) sw=true;
        if (sw) {
            Equipo* te=equipos[j]; equipos[j]=equipos[j+1]; equipos[j+1]=te;
            int tp=puntos[j]; puntos[j]=puntos[j+1]; puntos[j+1]=tp;
            int td=difGoles[j]; difGoles[j]=difGoles[j+1]; difGoles[j+1]=td;
            int tg=golesFavor[j]; golesFavor[j]=golesFavor[j+1]; golesFavor[j+1]=tg;
        }
    }
}

void Grupo::imprimirPartidos() {
    // FIX: ordenar partidos cronologicamente antes de imprimir
    Partido* ordenados[6];
    int cnt = 0;
    for (int i = 0; i < 6; i++) if (partidos[i]) ordenados[cnt++] = partidos[i];
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - 1 - i; j++)
            if (ordenados[j]->getFecha() > ordenados[j+1]->getFecha()) {
                Partido* tmp = ordenados[j];
                ordenados[j] = ordenados[j+1];
                ordenados[j+1] = tmp;
            }
    cout << "\n--- Partidos Grupo " << letra << " ---";
    for (int i = 0; i < cnt; i++) ordenados[i]->imprimirResumen();
}

void Grupo::imprimirTabla() {
    cout << "\n=== Grupo " << letra << " ===\nPos | Equipo                | Pts | DG  | GF\n----|----------------------|-----|-----|----";
    for (int i=0;i<cantEquipos;i++) {
        cout << "\n " << (i+1) << "  | " << equipos[i]->getPais()
             << " (" << equipos[i]->getConfederacion() << ")";
        int len = equipos[i]->getPais().length() + equipos[i]->getConfederacion().length() + 3;
        for(int s=len;s<21;s++) cout<<" ";
        cout << "| " << puntos[i] << "   | " << difGoles[i] << "   | " << golesFavor[i];
    }
    cout << endl;
}

Equipo** Grupo::obtenerClasificados() { return equipos; }
char     Grupo::getLetra()            { return letra; }
Equipo*  Grupo::getEquipo(int i)      { return (i>=0&&i<cantEquipos)?equipos[i]:nullptr; }
int      Grupo::getCantEquipos()      { return cantEquipos; }
int      Grupo::getPuntos(int i)      { return (i>=0&&i<4)?puntos[i]:0; }
int      Grupo::getDifGoles(int i)    { return (i>=0&&i<4)?difGoles[i]:0; }
int      Grupo::getGolesFavor(int i)  { return (i>=0&&i<4)?golesFavor[i]:0; }
Partido* Grupo::getPartido(int i)     { return (i>=0&&i<6)?partidos[i]:nullptr; }
