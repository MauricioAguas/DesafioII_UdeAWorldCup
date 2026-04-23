#include "../include/Grupo.h"
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

void Grupo::configurarPartidos(string fechaBase) {
    int e1idx[6]={0,0,0,1,1,2}, e2idx[6]={1,2,3,2,3,3};
    int offset = (letra-'A') % 4;
    int diasBase[3]={0+offset, 4+offset, 8+offset};
    int anio=stoi(fechaBase.substr(0,4)), mes=stoi(fechaBase.substr(5,2)), dia=stoi(fechaBase.substr(8,2));
    int diasMes[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    for (int p=0;p<6;p++) {
        int gf=p/2, dp=dia+diasBase[gf], mp=mes, ap=anio;
        while (dp>diasMes[mp]) { dp-=diasMes[mp]; mp++; if(mp>12){mp=1;ap++;} }
        string f=to_string(ap)+"-"+(mp<10?"0":"")+to_string(mp)+"-"+(dp<10?"0":"")+to_string(dp);
        partidos[p]=new Partido(f,"00:00","nombreSede",equipos[e1idx[p]],equipos[e2idx[p]]);
    }
}

void Grupo::simularPartidos() {
    for (int i=0;i<6;i++) if (partidos[i]) partidos[i]->simular();
}

void Grupo::generarTablaClasif() {
    for (int i=0;i<4;i++) puntos[i]=difGoles[i]=golesFavor[i]=0;
    for (int p=0;p<6;p++) {
        if (!partidos[p]) continue;
        Resultado* res=partidos[p]->getResultado();
        if (!res) continue;
        int gf1=res->getGfEquipo1(), gf2=res->getGfEquipo2();
        Equipo* eq1=partidos[p]->getEquipo1(), *eq2=partidos[p]->getEquipo2();
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

void Grupo::imprimirTabla() {
    cout << "\n=== Grupo " << letra << " ===\nPos | Equipo                | Pts | DG  | GF\n----|----------------------|-----|-----|----";
    for (int i=0;i<cantEquipos;i++) {
        cout << "\n " << (i+1) << "  | " << equipos[i]->getPais();
        int len=equipos[i]->getPais().length(); for(int s=len;s<21;s++) cout<<" ";
        cout << "| " << puntos[i] << "   | " << difGoles[i] << "   | " << golesFavor[i];
    }
    cout << endl;
}

Equipo** Grupo::obtenerClasificados() { return equipos; }
char     Grupo::getLetra()            { return letra; }
Equipo*  Grupo::getEquipo(int i)      { return (i>=0&&i<cantEquipos)?equipos[i]:nullptr; }
int      Grupo::getCantEquipos()      { return cantEquipos; }
int      Grupo::getPuntos(int i)      { return (i>=0&&i<4)?puntos[i]:0; }
