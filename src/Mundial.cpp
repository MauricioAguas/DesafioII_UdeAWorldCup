#include "../include/Mundial.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

Mundial::Mundial() {
    anio=cantEquipos=cantFases=0;
    for(int i=0;i<48;i++) equipos[i]=nullptr;
    for(int i=0;i<12;i++) grupos[i]=nullptr;
    for(int i=0;i<8;i++)  fases[i]=nullptr;
}

Mundial::Mundial(int anio, int cantEquipos) : Mundial() {
    this->anio=anio; this->cantEquipos=cantEquipos;
    srand(time(nullptr));
}

Mundial::~Mundial() {
    for(int i=0;i<cantEquipos;i++) if(equipos[i]) delete equipos[i];
    for(int i=0;i<12;i++)          if(grupos[i])  delete grupos[i];
    for(int i=0;i<cantFases;i++)   if(fases[i])   delete fases[i];
}

// ---------------------------------------------------------------
// Req I: Carga de equipos desde CSV
// Formato: pais,confederacion,ranking,DT,golesFavor,golesContra,
//          ganados,empatados,perdidos
// Tarjetas amarillas, rojas y faltas se inicializan en 0 (enunciado)
// ---------------------------------------------------------------
void Mundial::cargarEquipos() {
    ifstream archivo("data/selecciones_clasificadas_mundial.csv");
    if (!archivo.is_open()) {
        cerr << "[Error] No se pudo abrir data/selecciones_clasificadas_mundial.csv\n";
        return;
    }

    string linea;
    getline(archivo, linea); // saltar encabezado
    cantEquipos = 0;

    while (getline(archivo, linea) && cantEquipos < 48) {
        if (linea.empty()) continue;
        istringstream ss(linea);
        string pais, conf, rankStr, DT, gfStr, gcStr, ganStr, empStr, perStr;

        getline(ss, pais,    ',');
        getline(ss, conf,    ',');
        getline(ss, rankStr, ',');
        getline(ss, DT,      ',');
        getline(ss, gfStr,   ',');
        getline(ss, gcStr,   ',');
        getline(ss, ganStr,  ',');
        getline(ss, empStr,  ',');
        getline(ss, perStr,  ',');

        int ranking = stoi(rankStr);
        int gf      = stoi(gfStr);
        int gc      = stoi(gcStr);
        int gan     = stoi(ganStr);
        int emp     = stoi(empStr);
        int per     = stoi(perStr);

        Equipo* e = new Equipo(pais, conf, ranking, DT);
        // Establecer estadisticas historicas iniciales
        e->setEstadisticasIniciales(gf, gc, gan, emp, per);
        // Tarjetas y faltas iniciales = 0 (ya es el default en EstadisticasEquipo)

        // Crear plantilla con reparto uniforme de goles
        e->cargarJugadores();

        equipos[cantEquipos++] = e;
    }
    archivo.close();
    cout << "[Mundial] " << cantEquipos << " equipos cargados desde CSV.\n";
}

void Mundial::conformarGrupos() {
    char letras[12]={'A','B','C','D','E','F','G','H','I','J','K','L'};
    for(int g=0;g<12;g++) grupos[g]=new Grupo(letras[g]);
    // TODO: sorteo completo con bombos y restricciones de confederacion
    for(int g=0;g<12;g++)
        for(int e=0;e<4;e++)
            if(equipos[g*4+e]) grupos[g]->agregarEquipo(equipos[g*4+e]);
    cout << "[Mundial] Grupos conformados:\n";
    for(int g=0;g<12;g++) {
        cout << "Grupo " << letras[g] << ": ";
        for(int e=0;e<grupos[g]->getCantEquipos();e++) cout << grupos[g]->getEquipo(e)->getPais() << "  ";
        cout << "\n";
    }
}

void Mundial::simularTorneo() {
    Fase* faseGrupos=new Fase("Fase de Grupos","grupos","2026-06-20");
    for(int g=0;g<12;g++) faseGrupos->agregarGrupo(grupos[g]);
    faseGrupos->configurarPartidos();
    faseGrupos->simularPartidos();
    faseGrupos->generarTablas();
    faseGrupos->imprimirResultados();
    fases[cantFases++]=faseGrupos;

    Fase* faseR16=new Fase("Dieciseisavos (R16)","eliminacion","2026-07-10");
    armarR16(faseGrupos, faseR16);
    faseR16->simularPartidos();
    faseR16->imprimirResultados();
    fases[cantFases++]=faseR16;

    Fase* faseR8=new Fase("Octavos (R8)","eliminacion","2026-07-10");
    for(int i=0;i<faseR16->getCantPartidos();i+=2) {
        Equipo* g=faseR16->getGanadorPartido(i);
        Equipo* h=faseR16->getGanadorPartido(i+1);
        if(g&&h) faseR8->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseR8->simularPartidos(); faseR8->imprimirResultados();
    fases[cantFases++]=faseR8;

    Fase* faseQF=new Fase("Cuartos (QF)","eliminacion","2026-07-10");
    for(int i=0;i<faseR8->getCantPartidos();i+=2) {
        Equipo* g=faseR8->getGanadorPartido(i);
        Equipo* h=faseR8->getGanadorPartido(i+1);
        if(g&&h) faseQF->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseQF->simularPartidos(); faseQF->imprimirResultados();
    fases[cantFases++]=faseQF;

    Fase* faseSF=new Fase("Semifinales (SF)","eliminacion","2026-07-10");
    for(int i=0;i<faseQF->getCantPartidos();i+=2) {
        Equipo* g=faseQF->getGanadorPartido(i);
        Equipo* h=faseQF->getGanadorPartido(i+1);
        if(g&&h) faseSF->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseSF->simularPartidos(); faseSF->imprimirResultados();
    fases[cantFases++]=faseSF;

    Equipo* sf1g=faseSF->getGanadorPartido(0);
    Equipo* sf2g=faseSF->getGanadorPartido(1);
    Equipo* p0e1=faseSF->getPartido(0)?faseSF->getPartido(0)->getEquipo1():nullptr;
    Equipo* p0e2=faseSF->getPartido(0)?faseSF->getPartido(0)->getEquipo2():nullptr;
    Equipo* p1e1=faseSF->getPartido(1)?faseSF->getPartido(1)->getEquipo1():nullptr;
    Equipo* p1e2=faseSF->getPartido(1)?faseSF->getPartido(1)->getEquipo2():nullptr;
    Equipo* perd1=(sf1g==p0e1)?p0e2:p0e1;
    Equipo* perd2=(sf2g==p1e1)?p1e2:p1e1;

    Fase* fase3er=new Fase("Tercer Puesto","eliminacion","2026-07-10");
    if(perd1&&perd2) fase3er->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",perd1,perd2));
    fase3er->simularPartidos(); fase3er->imprimirResultados();
    fases[cantFases++]=fase3er;

    Fase* faseFinal=new Fase("Final","eliminacion","2026-07-10");
    if(sf1g&&sf2g) faseFinal->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",sf1g,sf2g));
    faseFinal->simularPartidos(); faseFinal->imprimirResultados();
    fases[cantFases++]=faseFinal;
}

void Mundial::armarR16(Fase* fg, Fase* r16) {
    // TODO: logica completa (8 mejores terceros + restricciones de confederacion)
    for(int g=0;g<fg->getCantGrupos();g+=2) {
        Grupo* ga=fg->getGrupo(g); Grupo* gb=fg->getGrupo(g+1);
        if(ga&&gb) {
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",ga->obtenerClasificados()[0],gb->obtenerClasificados()[1]));
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",gb->obtenerClasificados()[0],ga->obtenerClasificados()[1]));
        }
    }
}

void Mundial::generarEstadisticas() {
    cout << "\n========== ESTADISTICAS FINALES ==========\n";
    // TODO: implementar estadisticas completas del enunciado
    cout << "[Mundial] generarEstadisticas() pendiente de implementacion\n";
}

Equipo* Mundial::getCampeon() {
    for(int i=0;i<cantFases;i++)
        if(fases[i]&&fases[i]->getNombre()=="Final") return fases[i]->getGanadorPartido(0);
    return nullptr;
}

Equipo* Mundial::getEquipoMasGoles() {
    Equipo* mejor=nullptr; int max=-1;
    for(int i=0;i<cantEquipos;i++)
        if(equipos[i]&&equipos[i]->getGFA()>max) { max=equipos[i]->getGFA(); mejor=equipos[i]; }
    return mejor;
}
