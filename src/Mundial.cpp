#include "../hds/Mundial.h"
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

void Mundial::cargarEquipos() {
    ifstream archivo("data/selecciones_clasificadas_mundial.csv");
    if (!archivo.is_open()) {
        cerr << "[Error] No se pudo abrir data/selecciones_clasificadas_mundial.csv\n";
        return;
    }
    string linea;
    getline(archivo, linea);
    getline(archivo, linea);
    cantEquipos = 0;
    while (getline(archivo, linea) && cantEquipos < 48) {
        if (linea.empty()) continue;
        istringstream ss(linea);
        string rankStr, pais, DT, federacion, conf, gfStr, gcStr, ganStr, empStr, perStr;
        getline(ss, rankStr,   ',');
        getline(ss, pais,      ',');
        getline(ss, DT,        ',');
        getline(ss, federacion,',');
        getline(ss, conf,      ',');
        getline(ss, gfStr,     ',');
        getline(ss, gcStr,     ',');
        getline(ss, ganStr,    ',');
        getline(ss, empStr,    ',');
        getline(ss, perStr,    ',');
        int ranking = stoi(rankStr);
        int gf = stoi(gfStr), gc = stoi(gcStr);
        int gan = stoi(ganStr), emp = stoi(empStr), per = stoi(perStr);
        Equipo* e = new Equipo(pais, conf, ranking, DT);
        e->setEstadisticasIniciales(gf, gc, gan, emp, per);
        e->cargarJugadores();
        equipos[cantEquipos++] = e;
    }
    archivo.close();
    cout << "[Mundial] " << cantEquipos << " equipos cargados desde CSV.\n";
}

void Mundial::conformarGrupos() {
    char letras[12]={'A','B','C','D','E','F','G','H','I','J','K','L'};
    for(int g=0;g<12;g++) grupos[g]=new Grupo(letras[g]);
    for(int g=0;g<12;g++)
        for(int e=0;e<4;e++)
            if(equipos[g*4+e]) grupos[g]->agregarEquipo(equipos[g*4+e]);
    cout << "[Mundial] Grupos conformados:\n";
    for(int g=0;g<12;g++) {
        cout << "Grupo " << letras[g] << ": ";
        for(int e=0;e<grupos[g]->getCantEquipos();e++)
            cout << grupos[g]->getEquipo(e)->getPais()
                 << " (" << grupos[g]->getEquipo(e)->getConfederacion() << ")  ";
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
    for(int i=0;i+1<faseR16->getCantPartidos();i+=2) {
        Equipo* g=faseR16->getGanadorPartido(i);
        Equipo* h=faseR16->getGanadorPartido(i+1);
        if(g&&h) faseR8->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseR8->simularPartidos(); faseR8->imprimirResultados();
    fases[cantFases++]=faseR8;

    Fase* faseQF=new Fase("Cuartos (QF)","eliminacion","2026-07-10");
    for(int i=0;i+1<faseR8->getCantPartidos();i+=2) {
        Equipo* g=faseR8->getGanadorPartido(i);
        Equipo* h=faseR8->getGanadorPartido(i+1);
        if(g&&h) faseQF->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseQF->simularPartidos(); faseQF->imprimirResultados();
    fases[cantFases++]=faseQF;

    Fase* faseSF=new Fase("Semifinales (SF)","eliminacion","2026-07-10");
    for(int i=0;i+1<faseQF->getCantPartidos();i+=2) {
        Equipo* g=faseQF->getGanadorPartido(i);
        Equipo* h=faseQF->getGanadorPartido(i+1);
        if(g&&h) faseSF->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseSF->simularPartidos(); faseSF->imprimirResultados();
    fases[cantFases++]=faseSF;

    Equipo* sf1g = faseSF->getGanadorPartido(0);
    Equipo* sf2g = faseSF->getGanadorPartido(1);
    Equipo* p0e1 = faseSF->getPartido(0) ? faseSF->getPartido(0)->getEquipo1() : nullptr;
    Equipo* p0e2 = faseSF->getPartido(0) ? faseSF->getPartido(0)->getEquipo2() : nullptr;
    Equipo* p1e1 = faseSF->getPartido(1) ? faseSF->getPartido(1)->getEquipo1() : nullptr;
    Equipo* p1e2 = faseSF->getPartido(1) ? faseSF->getPartido(1)->getEquipo2() : nullptr;
    Equipo* perd1 = (sf1g&&p0e1&&p0e2) ? ((sf1g==p0e1)?p0e2:p0e1) : nullptr;
    Equipo* perd2 = (sf2g&&p1e1&&p1e2) ? ((sf2g==p1e1)?p1e2:p1e1) : nullptr;

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
    for(int g=0;g+1<fg->getCantGrupos();g+=2) {
        Grupo* ga=fg->getGrupo(g);
        Grupo* gb=fg->getGrupo(g+1);
        if(ga&&gb) {
            Equipo* g1a = ga->obtenerClasificados()[0];
            Equipo* g2a = ga->obtenerClasificados()[1];
            Equipo* g1b = gb->obtenerClasificados()[0];
            Equipo* g2b = gb->obtenerClasificados()[1];
            if(g1a&&g2b) r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g1a,g2b));
            if(g1b&&g2a) r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g1b,g2a));
        }
    }
    int terceroIdx[4] = {0, 1, 2, 3};
    int segundoIdx[4] = {4, 5, 6, 7};
    for(int k=0;k<4;k++) {
        Grupo* gt = fg->getGrupo(terceroIdx[k]);
        Grupo* gs = fg->getGrupo(segundoIdx[k]);
        if(gt&&gs) {
            Equipo* t3 = gt->obtenerClasificados()[2];
            Equipo* s2 = gs->obtenerClasificados()[0];
            if(t3&&s2) r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",t3,s2));
        }
    }
}

void Mundial::generarEstadisticas() {
    cout << "\n========== ESTADISTICAS FINALES ==========\n";

    Equipo* campeon    = getCampeon();
    Equipo* subcampeon = nullptr;
    Equipo* tercero    = nullptr;
    Equipo* cuarto     = nullptr;

    for(int i=0;i<cantFases;i++) {
        if(fases[i] && fases[i]->getNombre()=="Final") {
            Partido* pf = fases[i]->getPartido(0);
            if(pf) {
                Equipo* e1=pf->getEquipo1(), *e2=pf->getEquipo2();
                subcampeon = (campeon==e1) ? e2 : e1;
            }
        }
        if(fases[i] && fases[i]->getNombre()=="Tercer Puesto") {
            tercero = fases[i]->getGanadorPartido(0);
            Partido* p3 = fases[i]->getPartido(0);
            if(p3) {
                Equipo* e1=p3->getEquipo1(), *e2=p3->getEquipo2();
                cuarto = (tercero==e1) ? e2 : e1;
            }
        }
    }

    cout << "\n--- Ranking Final ---\n";
    cout << "1er lugar: " << (campeon    ? campeon->getPais()    : "N/A") << "\n";
    cout << "2do lugar: " << (subcampeon ? subcampeon->getPais() : "N/A") << "\n";
    cout << "3er lugar: " << (tercero    ? tercero->getPais()    : "N/A") << "\n";
    cout << "4to lugar: " << (cuarto     ? cuarto->getPais()     : "N/A") << "\n";

    // Maximo goleador del campeon (recorre los 26 jugadores de la plantilla)
    cout << "\n--- Maximo Goleador del Campeon (" << (campeon?campeon->getPais():"N/A") << ") ---\n";
    if(campeon) {
        Jugador* maxJug = nullptr; int maxG = -1;
        for(int j=0; j<campeon->getTamanoPlantilla(); j++) {
            Jugador* jug = campeon->getJugador(j);
            if(jug && jug->getGoles() > maxG) { maxG = jug->getGoles(); maxJug = jug; }
        }
        if(maxJug)
            cout << maxJug->getNombre() << " " << maxJug->getApellido()
                 << " (#" << maxJug->getNumeroCamiseta() << ") - " << maxG << " goles\n";
    }

    // Top 3 goleadores del torneo (todos los equipos, todos los jugadores)
    cout << "\n--- Top 3 Goleadores del Torneo ---\n";
    Jugador* top[3] = {nullptr, nullptr, nullptr};
    int topG[3]     = {-1, -1, -1};
    Equipo*  topEq[3] = {nullptr, nullptr, nullptr};
    for(int e=0; e<cantEquipos; e++) {
        if(!equipos[e]) continue;
        for(int j=0; j<equipos[e]->getTamanoPlantilla(); j++) {
            Jugador* jug = equipos[e]->getJugador(j);
            if(!jug) continue;
            int g = jug->getGoles();
            // Insertar en top 3
            for(int k=0; k<3; k++) {
                if(g > topG[k]) {
                    // Desplazar
                    for(int m=2; m>k; m--) { top[m]=top[m-1]; topG[m]=topG[m-1]; topEq[m]=topEq[m-1]; }
                    top[k]=jug; topG[k]=g; topEq[k]=equipos[e];
                    break;
                }
            }
        }
    }
    for(int k=0; k<3; k++) {
        if(top[k])
            cout << (k+1) << ". " << top[k]->getNombre() << " " << top[k]->getApellido()
                 << " (#" << top[k]->getNumeroCamiseta() << ") - "
                 << topEq[k]->getPais() << " - " << topG[k] << " goles\n";
    }

    // Equipo con mas goles historicos
    cout << "\n--- Equipo con mas goles historicos (post-copa) ---\n";
    Equipo* masGoles = getEquipoMasGoles();
    if(masGoles)
        cout << masGoles->getPais() << " con " << masGoles->getGFA() << " goles a favor historicos\n";

    // Confederacion con mayor presencia en cada etapa
    const char* etapas[3] = {"Dieciseisavos (R16)", "Octavos (R8)", "Cuartos (QF)"};
    string confNombres[6] = {"UEFA","CONMEBOL","CONCACAF","CAF","AFC","OFC"};
    for(int e=0;e<3;e++) {
        int confCount[6] = {0};
        for(int f=0;f<cantFases;f++) {
            if(!fases[f] || fases[f]->getNombre()!=etapas[e]) continue;
            for(int p=0;p<fases[f]->getCantPartidos();p++) {
                Partido* par = fases[f]->getPartido(p);
                if(!par) continue;
                Equipo* eq[2] = {par->getEquipo1(), par->getEquipo2()};
                for(int q=0;q<2;q++) {
                    if(!eq[q]) continue;
                    for(int c=0;c<6;c++)
                        if(eq[q]->getConfederacion()==confNombres[c]) { confCount[c]++; break; }
                }
            }
        }
        string maxConf=""; int maxC=0;
        for(int c=0;c<6;c++) if(confCount[c]>maxC) { maxC=confCount[c]; maxConf=confNombres[c]; }
        cout << "\nConfederacion con mayor presencia en " << etapas[e]
             << ": " << maxConf << " (" << maxC << " equipos)\n";
    }
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
