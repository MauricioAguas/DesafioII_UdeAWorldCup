#include "../hds/Mundial.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
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

// -----------------------------------------------------------------------
// conformarGrupos: sorteo por bombos con restricciones de confederacion
//
// Bombo 1 (12 cabezas de serie): los 12 equipos de menor ranking (mejor
//         ranking FIFA = numero mas bajo).  Uno por grupo, sin restriccion
//         de confederacion (el cabeza de serie de anfitrion va al grupo A).
// Bombos 2, 3 y 4 (12 equipos cada uno): resto ordenados por ranking.
//         Restricciones al colocar un equipo en un grupo:
//           - Max 1 equipo de CONMEBOL por grupo.
//           - Max 1 equipo de CAF por grupo.
//           - Max 1 equipo de AFC por grupo.
//           - Max 1 equipo de OFC por grupo.
//           - Max 1 equipo de Concacaf por grupo.
//           - Max 3 equipos de UEFA por grupo (hay 16 UEFA para 12 grupos).
// -----------------------------------------------------------------------
static int contarConf(Grupo* g, const string& conf) {
    int c = 0;
    for(int i = 0; i < g->getCantEquipos(); i++)
        if(g->getEquipo(i) && g->getEquipo(i)->getConfederacion() == conf) c++;
    return c;
}

static bool puedeEntrar(Grupo* g, Equipo* e) {
    string conf = e->getConfederacion();
    int limite = (conf == "UEFA") ? 2 : 1;
    return contarConf(g, conf) < limite;
}

// FIX Bug 1: el fallback tambien verifica que el equipo no este ya en el grupo
static bool equipoYaEnGrupo(Grupo* g, Equipo* e) {
    for(int i = 0; i < g->getCantEquipos(); i++)
        if(g->getEquipo(i) == e) return true;
    return false;
}

void Mundial::conformarGrupos() {
    char letras[12]={'A','B','C','D','E','F','G','H','I','J','K','L'};
    for(int g=0;g<12;g++) grupos[g]=new Grupo(letras[g]);
    // Ordenar equipos por ranking (menor = mejor)
    // Usamos indices para no mover punteros originales
    int idx[48];
    for(int i=0;i<cantEquipos;i++) idx[i]=i;
    // Bubble sort por ranking
    for(int i=0;i<cantEquipos-1;i++)
        for(int j=0;j<cantEquipos-1-i;j++)
            if(equipos[idx[j]]->getRanking() > equipos[idx[j+1]]->getRanking())
                swap(idx[j], idx[j+1]);

    // Separar en 4 bombos de 12
    int bombos[4][12];
    for(int b=0;b<4;b++)
        for(int i=0;i<12;i++)
            bombos[b][i] = idx[b*12+i];

    // Bombo 1: mezclar y asignar un cabeza de serie por grupo
    for(int i=11;i>0;i--) {
        int j = rand()%(i+1);
        swap(bombos[0][i], bombos[0][j]);
    }
    for(int g=0;g<12;g++)
        grupos[g]->agregarEquipo(equipos[bombos[0][g]]);

    // Bombos 2, 3, 4: sorteo aleatorio con restricciones
    for(int b=1;b<4;b++) {
        // Mezclar bombo
        for(int i=11;i>0;i--) {
            int j = rand()%(i+1);
            swap(bombos[b][i], bombos[b][j]);
        }
        // Para cada equipo del bombo, buscar un grupo valido aleatorio
        for(int e=0;e<12;e++) {
            Equipo* eq = equipos[bombos[b][e]];
            // Grupos disponibles con restriccion de confederacion
            int disponibles[12]; int nd=0;
            for(int g=0;g<12;g++)
                if(grupos[g]->getCantEquipos()<4
                    && !equipoYaEnGrupo(grupos[g], eq)
                    && puedeEntrar(grupos[g], eq))
                    disponibles[nd++]=g;
            if(nd==0) { //fallback tambien excluye duplicados de equipo
                for(int g=0;g<12;g++)
                    if(grupos[g]->getCantEquipos()<4
                        && !equipoYaEnGrupo(grupos[g], eq))
                        disponibles[nd++]=g;
            }
            if(nd==0) continue; // seguridad: no deberia ocurrir
            int elegido = disponibles[rand()%nd];
            grupos[elegido]->agregarEquipo(eq);
        }
    }

    cout << "[Mundial] Grupos conformados por bombos:\n";
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

// armarR16: bracket oficial FIFA Mundial 2026 (48 equipos, 12 grupos)
//
// Clasifican: 1ro y 2do de cada grupo (24 equipos) +
//             8 mejores terceros de los 12 grupos (8 equipos) = 32 equipos
//
// Criterio para elegir los 8 mejores terceros:
//   1. Puntos, 2. Diferencia de goles, 3. Goles a favor
//
// Bracket (16 partidos):
//   6 pares de grupos: (A,B),(C,D),(E,F),(G,H),(I,J),(K,L)
//     Partido 2k-1 : 1ro_GrupoX  vs 2do_GrupoY
//     Partido 2k   : 1ro_GrupoY  vs 2do_GrupoX
//   12 partidos con los 24 clasificados directos.
//
//   Luego los 8 mejores terceros se enfrentan a 1eros de grupos que
//   NO participaron en el mismo par que el grupo del tercero, evitando
//   que un equipo juegue contra alguien de su propio grupo (self-match).

//   - Se rastrea que equipos ya tienen partido asignado en R16 para
//     evitar duplicados.
//   - El rival del mejor-tercero se elige entre los 1eros disponibles
//     (no usados aun), garantizando que no sea del mismo grupo.
// -----------------------------------------------------------------------
static bool mejorTercero(Grupo* a, Grupo* b) {
    // Compara el 3er clasificado de cada grupo
    Equipo** clA = a->obtenerClasificados();
    Equipo** clB = b->obtenerClasificados();
    if(!clA[2] || !clB[2]) return false;
    int idxA = -1, idxB = -1;
    for(int i=0;i<a->getCantEquipos();i++) if(a->getEquipo(i)==clA[2]) { idxA=i; break; }
    for(int i=0;i<b->getCantEquipos();i++) if(b->getEquipo(i)==clB[2]) { idxB=i; break; }
    if(idxA<0||idxB<0) return false;
    if(a->getPuntos(idxA) != b->getPuntos(idxB))
        return a->getPuntos(idxA) > b->getPuntos(idxB);
    if(a->getDifGoles(idxA) != b->getDifGoles(idxB))
        return a->getDifGoles(idxA) > b->getDifGoles(idxB);
    return a->getGolesFavor(idxA) > b->getGolesFavor(idxB);
}

void Mundial::armarR16(Fase* fg, Fase* r16) {
    int cantG = fg->getCantGrupos();

    // Rastrear equipos ya asignados
    Equipo* enR16[48]; int nR16 = 0;
    auto yaEnR16 = [&](Equipo* e) -> bool {
        for(int i=0;i<nR16;i++) if(enR16[i]==e) return true;
        return false;
    };
    auto registrar = [&](Equipo* e) {
        if(e && !yaEnR16(e)) enR16[nR16++] = e;
    };

    // --- 12 partidos: pares de grupos adyacentes (A,B),(C,D),(E,F),(G,H),(I,J),(K,L) ---
    for(int g=0; g+1<cantG; g+=2) {
        Grupo* ga = fg->getGrupo(g);
        Grupo* gb = fg->getGrupo(g+1);
        if(!ga || !gb) continue;
        Equipo* g1a = ga->obtenerClasificados()[0];
        Equipo* g2a = ga->obtenerClasificados()[1];
        Equipo* g1b = gb->obtenerClasificados()[0];
        Equipo* g2b = gb->obtenerClasificados()[1];
        if(g1a && g2b && !yaEnR16(g1a) && !yaEnR16(g2b)) {
            r16->agregarPartido(new Partido("2026-07-04","00:00","nombreSede",g1a,g2b));
            registrar(g1a); registrar(g2b);
        }
        if(g1b && g2a && !yaEnR16(g1b) && !yaEnR16(g2a)) {
            r16->agregarPartido(new Partido("2026-07-05","00:00","nombreSede",g1b,g2a));
            registrar(g1b); registrar(g2a);
        }
    }

    // --- Seleccionar los 8 mejores terceros ---
    int ordenGrupos[12];
    for(int i=0;i<cantG;i++) ordenGrupos[i]=i;
    for(int i=0;i<cantG-1;i++) {
        for(int j=0;j<cantG-1-i;j++) {
            Grupo* ga = fg->getGrupo(ordenGrupos[j]);
            Grupo* gb = fg->getGrupo(ordenGrupos[j+1]);
            if(ga && gb && mejorTercero(gb, ga))
                swap(ordenGrupos[j], ordenGrupos[j+1]);
        }
    }

    // Recolectar los 8 mejores terceros disponibles
    Equipo* terceros[8]; int nT = 0;
    int grupoDelTercero[8];
    for(int k=0; k<cantG && nT<8; k++) {
        Grupo* gt = fg->getGrupo(ordenGrupos[k]);
        if(!gt) continue;
        Equipo* t3 = gt->obtenerClasificados()[2];
        if(t3 && !yaEnR16(t3)) {
            terceros[nT] = t3;
            grupoDelTercero[nT] = ordenGrupos[k];
            nT++;
        }
    }

    // Recolectar los 1eros que NO tienen partido aun (los 4 que quedaron fuera de los pares)
    // Con 12 grupos y 6 pares, los 12 primeros ya estan en R16.
    // Los 8 terceros se enfrentan a los 8 segundos sobrantes? No:
    // Los 24 clasificados directos (12 primeros + 12 segundos) ya estan asignados en los 12 partidos.
    // Los 8 terceros necesitan 8 rivales: son los 8 primeros de grupos que
    // "ceden" su primer puesto — pero ya estan usados.
    // CORRECION: el bracket real enfrenta terceros contra primeros de grupos
    // que NO son del mismo par. Buscamos 1eros no usados — si todos estan
    // usados, enfrentamos terceros entre si.
    for(int t=0; t<nT; t++) {
        Equipo* t3 = terceros[t];
        if(yaEnR16(t3)) continue;

        // Buscar un rival disponible: primero intentar 1eros no usados de grupo distinto
        Equipo* rival = nullptr;
        for(int r=0; r<cantG && !rival; r++) {
            if(r == grupoDelTercero[t]) continue;
            Grupo* gr = fg->getGrupo(r);
            if(!gr) continue;
            Equipo* r1 = gr->obtenerClasificados()[0];
            if(r1 && !yaEnR16(r1)) { rival = r1; }
        }
        // Si no hay 1ero disponible, buscar 2do no usado de grupo distinto
        for(int r=0; r<cantG && !rival; r++) {
            if(r == grupoDelTercero[t]) continue;
            Grupo* gr = fg->getGrupo(r);
            if(!gr) continue;
            Equipo* r2 = gr->obtenerClasificados()[1];
            if(r2 && !yaEnR16(r2)) { rival = r2; }
        }
        // Ultimo recurso: otro tercero disponible de grupo distinto
        for(int u=0; u<nT && !rival; u++) {
            if(u == t) continue;
            if(grupoDelTercero[u] == grupoDelTercero[t]) continue;
            if(!yaEnR16(terceros[u])) { rival = terceros[u]; }
        }
        if(t3 && rival) {
            r16->agregarPartido(new Partido("2026-07-06","00:00","nombreSede",t3,rival));
            registrar(t3); registrar(rival);
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
            for(int k=0; k<3; k++) {
                if(g > topG[k]) {
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

    cout << "\n--- Equipo con mas goles historicos (post-copa) ---\n";
    Equipo* masGoles = getEquipoMasGoles();
    if(masGoles)
        cout << masGoles->getPais() << " con " << masGoles->getGFA() << " goles a favor historicos\n";

    // Usar el mismo string que viene del CSV para Concacaf
    const char* etapas[3] = {"Dieciseisavos (R16)", "Octavos (R8)", "Cuartos (QF)"};
    string confNombres[6] = {"UEFA","CONMEBOL","Concacaf","CAF","AFC","OFC"};
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
