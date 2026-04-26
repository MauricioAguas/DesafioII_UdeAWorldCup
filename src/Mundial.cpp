#include "../hds/Mundial.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;

// ============================================================
// HELPERS PRIVADOS PARA CALCULO DE MEMORIA
// ============================================================

// Bytes ocupados por un std::string en heap (aproximacion portátil):
// sizeof(string) cubre el objeto en sí; capacity()+1 es el buffer externo
// cuando la cadena supera el SSO (Small String Optimization ~15 chars).
static long long bytesString(const string& s) {
    long long b = sizeof(string);
    if (s.capacity() > 15) b += (long long)(s.capacity() + 1);
    return b;
}

// Bytes de un EstadisticasJugador en heap
static long long bytesEstJug() {
    return sizeof(EstadisticasJugador);
}

// Bytes de un Jugador en heap (objeto + puntero a EstadisticasJugador)
static long long bytesJugador(const Jugador* j) {
    if (!j) return 0;
    return sizeof(Jugador)
         + bytesString(j->getNombre())     // pero getNombre() devuelve por valor...
         // Los strings nombre/apellido son miembros por valor dentro de Jugador
         // => ya los cuenta sizeof(Jugador); solo sumamos buffer externo si aplica
         + bytesEstJug();
}

// Bytes de EstadisticasEquipo en heap
static long long bytesEstEq() {
    return sizeof(EstadisticasEquipo);
}

// Bytes de un Equipo en heap (sin contar los Jugador* — se cuentan aparte)
static long long bytesEquipo(const Equipo* e) {
    if (!e) return 0;
    // sizeof(Equipo) incluye los strings pais/confederacion/DT por valor
    // y el array de punteros plantilla[26] + puntero estadisticas
    return sizeof(Equipo) + bytesEstEq();
}

// Bytes de un Resultado en heap
static long long bytesResultado(const Resultado* r) {
    if (!r) return 0;
    return sizeof(Resultado);
}

// Bytes de un Partido en heap
static long long bytesPartido(const Partido* p) {
    if (!p) return 0;
    return sizeof(Partido) + bytesResultado(p->getResultado());
}

// Bytes de un Grupo en heap (sin los Partido* — se cuentan en Fase)
static long long bytesGrupo(const Grupo* g) {
    if (!g) return 0;
    return sizeof(Grupo);
}

// Bytes de una Fase en heap
static long long bytesFase(const Fase* f) {
    if (!f) return 0;
    long long b = sizeof(Fase);
    for (int p = 0; p < f->getCantPartidos(); p++)
        b += bytesPartido(f->getPartido(p));
    return b;
}

// ============================================================
// calcularMemoria: suma todos los objetos vivos en heap
// ============================================================
long long Mundial::calcularMemoria() const {
    long long total = sizeof(Mundial); // el propio objeto Mundial (en stack en main)

    // Equipos y sus jugadores
    for (int i = 0; i < cantEquipos; i++) {
        if (!equipos[i]) continue;
        total += bytesEquipo(equipos[i]);
        for (int j = 0; j < equipos[i]->getTamanoPlantilla(); j++)
            total += bytesJugador(equipos[i]->getJugador(j));
    }

    // Grupos (los partidos de grupos están dentro de Fase faseGrupos)
    for (int g = 0; g < 12; g++)
        total += bytesGrupo(grupos[g]);

    // Fases y sus partidos
    for (int f = 0; f < cantFases; f++)
        total += bytesFase(fases[f]);

    return total;
}

// ============================================================
// imprimirMetricas
// ============================================================
void Mundial::imprimirMetricas(const string& nombreFunc) const {
    cout << "\n[Metricas - " << nombreFunc << "]\n";
    cout << "  Iteraciones ejecutadas : " << iteraciones << "\n";
    cout << "  Memoria consumida      : " << calcularMemoria() << " bytes\n";
}

// ============================================================
// CONSTRUCTORES / DESTRUCTOR
// ============================================================
Mundial::Mundial() {
    anio = cantEquipos = cantFases = 0;
    iteraciones = 0;
    for (int i = 0; i < 48; i++) equipos[i] = nullptr;
    for (int i = 0; i < 12; i++) grupos[i]  = nullptr;
    for (int i = 0; i < 8;  i++) fases[i]   = nullptr;
}

Mundial::Mundial(int anio, int cantEquipos) : Mundial() {
    this->anio        = anio;
    this->cantEquipos = cantEquipos;
    srand(time(nullptr));
}

Mundial::~Mundial() {
    for (int i = 0; i < cantEquipos; i++) if (equipos[i]) delete equipos[i];
    for (int i = 0; i < 12; i++)          if (grupos[i])  delete grupos[i];
    for (int i = 0; i < cantFases; i++)   if (fases[i])   delete fases[i];
}

// ============================================================
// Req I: cargarEquipos
// ============================================================
void Mundial::cargarEquipos() {
    iteraciones = 0;

    ifstream archivo("data/selecciones_clasificadas_mundial.csv");
    if (!archivo.is_open()) {
        cerr << "[Error] No se pudo abrir data/selecciones_clasificadas_mundial.csv\n";
        return;
    }
    string linea;
    getline(archivo, linea); iteraciones++; // saltar fila titulo
    getline(archivo, linea); iteraciones++; // saltar fila encabezado
    cantEquipos = 0;

    while (getline(archivo, linea) && cantEquipos < 48) {
        iteraciones++;
        if (linea.empty()) continue;
        istringstream ss(linea);
        string rankStr, pais, DT, federacion, conf, gfStr, gcStr, ganStr, empStr, perStr;
        getline(ss, rankStr,    ','); iteraciones++;
        getline(ss, pais,       ','); iteraciones++;
        getline(ss, DT,         ','); iteraciones++;
        getline(ss, federacion, ','); iteraciones++;
        getline(ss, conf,       ','); iteraciones++;
        getline(ss, gfStr,      ','); iteraciones++;
        getline(ss, gcStr,      ','); iteraciones++;
        getline(ss, ganStr,     ','); iteraciones++;
        getline(ss, empStr,     ','); iteraciones++;
        getline(ss, perStr,     ','); iteraciones++;

        int ranking = stoi(rankStr);
        int gf = stoi(gfStr), gc = stoi(gcStr);
        int gan = stoi(ganStr), emp = stoi(empStr), per = stoi(perStr);
        iteraciones += 6;

        Equipo* e = new Equipo(pais, conf, ranking, DT);
        e->setEstadisticasIniciales(gf, gc, gan, emp, per); iteraciones++;
        // cargarJugadores hace 26 iteraciones internas
        e->cargarJugadores(); iteraciones += 26;
        equipos[cantEquipos++] = e;
    }
    archivo.close();
    cout << "[Mundial] " << cantEquipos << " equipos cargados desde CSV.\n";
    imprimirMetricas("cargarEquipos");
}

// ============================================================
// helpers estaticos para conformarGrupos (sin cambios logicos)
// ============================================================
static int contarConf(Grupo* g, const string& conf) {
    int c = 0;
    for (int i = 0; i < g->getCantEquipos(); i++)
        if (g->getEquipo(i) && g->getEquipo(i)->getConfederacion() == conf) c++;
    return c;
}
static bool puedeEntrar(Grupo* g, Equipo* e) {
    string conf = e->getConfederacion();
    int limite  = (conf == "UEFA") ? 2 : 1;
    return contarConf(g, conf) < limite;
}
static bool equipoYaEnGrupo(Grupo* g, Equipo* e) {
    for (int i = 0; i < g->getCantEquipos(); i++)
        if (g->getEquipo(i) == e) return true;
    return false;
}

// ============================================================
// Req II: conformarGrupos
// ============================================================
void Mundial::conformarGrupos() {
    iteraciones = 0;

    char letras[12] = {'A','B','C','D','E','F','G','H','I','J','K','L'};
    for (int g = 0; g < 12; g++) { grupos[g] = new Grupo(letras[g]); iteraciones++; }

    int idx[48];
    for (int i = 0; i < cantEquipos; i++) { idx[i] = i; iteraciones++; }

    // Bubble sort por ranking
    for (int i = 0; i < cantEquipos - 1; i++) {
        for (int j = 0; j < cantEquipos - 1 - i; j++) {
            iteraciones++;
            if (equipos[idx[j]]->getRanking() > equipos[idx[j+1]]->getRanking())
                swap(idx[j], idx[j+1]);
        }
    }

    int bombos[4][12];
    for (int b = 0; b < 4; b++)
        for (int i = 0; i < 12; i++) { bombos[b][i] = idx[b*12+i]; iteraciones++; }

    // Bombo 1: mezclar y asignar
    for (int i = 11; i > 0; i--) {
        int j = rand() % (i+1);
        swap(bombos[0][i], bombos[0][j]);
        iteraciones++;
    }
    for (int g = 0; g < 12; g++) { grupos[g]->agregarEquipo(equipos[bombos[0][g]]); iteraciones++; }

    // Bombos 2, 3, 4
    for (int b = 1; b < 4; b++) {
        for (int i = 11; i > 0; i--) {
            int j = rand() % (i+1);
            swap(bombos[b][i], bombos[b][j]);
            iteraciones++;
        }
        for (int e = 0; e < 12; e++) {
            iteraciones++;
            Equipo* eq = equipos[bombos[b][e]];
            int disponibles[12]; int nd = 0;
            for (int g = 0; g < 12; g++) {
                iteraciones++;
                if (grupos[g]->getCantEquipos() < 4
                    && !equipoYaEnGrupo(grupos[g], eq)
                    && puedeEntrar(grupos[g], eq))
                    disponibles[nd++] = g;
            }
            if (nd == 0) {
                for (int g = 0; g < 12; g++) {
                    iteraciones++;
                    if (grupos[g]->getCantEquipos() < 4
                        && !equipoYaEnGrupo(grupos[g], eq))
                        disponibles[nd++] = g;
                }
            }
            if (nd == 0) continue;
            int elegido = disponibles[rand() % nd];
            grupos[elegido]->agregarEquipo(eq);
        }
    }

    cout << "[Mundial] Grupos conformados por bombos:\n";
    for (int g = 0; g < 12; g++) {
        cout << "Grupo " << letras[g] << ": ";
        for (int e = 0; e < grupos[g]->getCantEquipos(); e++) {
            iteraciones++;
            cout << grupos[g]->getEquipo(e)->getPais()
                 << " (" << grupos[g]->getEquipo(e)->getConfederacion() << ")  ";
        }
        cout << "\n";
    }
    imprimirMetricas("conformarGrupos");
}

// ============================================================
// Req III: simularTorneo
// ============================================================
void Mundial::simularTorneo() {
    iteraciones = 0;

    Fase* faseGrupos = new Fase("Fase de Grupos", "grupos", "2026-06-20");
    for (int g = 0; g < 12; g++) { faseGrupos->agregarGrupo(grupos[g]); iteraciones++; }
    faseGrupos->configurarPartidos(); iteraciones += 72;  // 12 grupos x 6 partidos
    faseGrupos->simularPartidos();    iteraciones += 72;  // 72 partidos simulados
    faseGrupos->generarTablas();      iteraciones += 48;  // ordenar 4 equipos x 12 grupos
    faseGrupos->imprimirResultados();
    fases[cantFases++] = faseGrupos;

    Fase* faseR16 = new Fase("Dieciseisavos (R16)", "eliminacion", "2026-07-10");
    armarR16(faseGrupos, faseR16);    // iteraciones se acumulan dentro
    faseR16->simularPartidos();       iteraciones += 16;
    faseR16->imprimirResultados();
    fases[cantFases++] = faseR16;

    // R8
    Fase* faseR8 = new Fase("Octavos (R8)", "eliminacion", "2026-07-10");
    for (int i = 0; i + 1 < faseR16->getCantPartidos(); i += 2) {
        iteraciones++;
        Equipo* g = faseR16->getGanadorPartido(i);
        Equipo* h = faseR16->getGanadorPartido(i+1);
        if (g && h) faseR8->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseR8->simularPartidos(); iteraciones += 8;
    faseR8->imprimirResultados();
    fases[cantFases++] = faseR8;

    // QF
    Fase* faseQF = new Fase("Cuartos (QF)", "eliminacion", "2026-07-10");
    for (int i = 0; i + 1 < faseR8->getCantPartidos(); i += 2) {
        iteraciones++;
        Equipo* g = faseR8->getGanadorPartido(i);
        Equipo* h = faseR8->getGanadorPartido(i+1);
        if (g && h) faseQF->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseQF->simularPartidos(); iteraciones += 4;
    faseQF->imprimirResultados();
    fases[cantFases++] = faseQF;

    // SF
    Fase* faseSF = new Fase("Semifinales (SF)", "eliminacion", "2026-07-10");
    for (int i = 0; i + 1 < faseQF->getCantPartidos(); i += 2) {
        iteraciones++;
        Equipo* g = faseQF->getGanadorPartido(i);
        Equipo* h = faseQF->getGanadorPartido(i+1);
        if (g && h) faseSF->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g,h));
    }
    faseSF->simularPartidos(); iteraciones += 2;
    faseSF->imprimirResultados();
    fases[cantFases++] = faseSF;

    // Tercer puesto
    Equipo* sf1g = faseSF->getGanadorPartido(0);
    Equipo* sf2g = faseSF->getGanadorPartido(1);
    Partido* p0  = faseSF->getPartido(0);
    Partido* p1  = faseSF->getPartido(1);
    Equipo* p0e1 = p0 ? p0->getEquipo1() : nullptr;
    Equipo* p0e2 = p0 ? p0->getEquipo2() : nullptr;
    Equipo* p1e1 = p1 ? p1->getEquipo1() : nullptr;
    Equipo* p1e2 = p1 ? p1->getEquipo2() : nullptr;
    Equipo* perd1 = (sf1g && p0e1 && p0e2) ? ((sf1g==p0e1) ? p0e2 : p0e1) : nullptr;
    Equipo* perd2 = (sf2g && p1e1 && p1e2) ? ((sf2g==p1e1) ? p1e2 : p1e1) : nullptr;

    Fase* fase3er = new Fase("Tercer Puesto", "eliminacion", "2026-07-10");
    if (perd1 && perd2)
        fase3er->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",perd1,perd2));
    fase3er->simularPartidos(); iteraciones++;
    fase3er->imprimirResultados();
    fases[cantFases++] = fase3er;

    // Final
    Fase* faseFinal = new Fase("Final", "eliminacion", "2026-07-10");
    if (sf1g && sf2g)
        faseFinal->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",sf1g,sf2g));
    faseFinal->simularPartidos(); iteraciones++;
    faseFinal->imprimirResultados();
    fases[cantFases++] = faseFinal;

    imprimirMetricas("simularTorneo");
}

// ============================================================
// helpers armarR16
// ============================================================
static bool mejorTercero(Grupo* a, Grupo* b) {
    Equipo** clA = a->obtenerClasificados();
    Equipo** clB = b->obtenerClasificados();
    if (!clA[2] || !clB[2]) return false;
    int idxA = -1, idxB = -1;
    for (int i = 0; i < a->getCantEquipos(); i++) if (a->getEquipo(i)==clA[2]) { idxA=i; break; }
    for (int i = 0; i < b->getCantEquipos(); i++) if (b->getEquipo(i)==clB[2]) { idxB=i; break; }
    if (idxA < 0 || idxB < 0) return false;
    if (a->getPuntos(idxA) != b->getPuntos(idxB))
        return a->getPuntos(idxA) > b->getPuntos(idxB);
    if (a->getDifGoles(idxA) != b->getDifGoles(idxB))
        return a->getDifGoles(idxA) > b->getDifGoles(idxB);
    return a->getGolesFavor(idxA) > b->getGolesFavor(idxB);
}

void Mundial::armarR16(Fase* fg, Fase* r16) {
    int cantG = fg->getCantGrupos();
    Equipo* enR16[48]; int nR16 = 0;

    auto yaEnR16 = [&](Equipo* e) -> bool {
        for (int i = 0; i < nR16; i++) { iteraciones++; if (enR16[i]==e) return true; }
        return false;
    };
    auto registrar = [&](Equipo* e) {
        if (e && !yaEnR16(e)) enR16[nR16++] = e;
    };

    // 12 partidos: pares de grupos
    for (int g = 0; g + 1 < cantG; g += 2) {
        iteraciones++;
        Grupo* ga = fg->getGrupo(g);
        Grupo* gb = fg->getGrupo(g+1);
        if (!ga || !gb) continue;
        Equipo* g1a = ga->obtenerClasificados()[0];
        Equipo* g2a = ga->obtenerClasificados()[1];
        Equipo* g1b = gb->obtenerClasificados()[0];
        Equipo* g2b = gb->obtenerClasificados()[1];
        if (g1a && g2b && !yaEnR16(g1a) && !yaEnR16(g2b)) {
            r16->agregarPartido(new Partido("2026-07-04","00:00","nombreSede",g1a,g2b));
            registrar(g1a); registrar(g2b);
        }
        if (g1b && g2a && !yaEnR16(g1b) && !yaEnR16(g2a)) {
            r16->agregarPartido(new Partido("2026-07-05","00:00","nombreSede",g1b,g2a));
            registrar(g1b); registrar(g2a);
        }
    }

    // Seleccionar 8 mejores terceros
    int ordenGrupos[12];
    for (int i = 0; i < cantG; i++) { ordenGrupos[i] = i; iteraciones++; }
    for (int i = 0; i < cantG-1; i++) {
        for (int j = 0; j < cantG-1-i; j++) {
            iteraciones++;
            Grupo* ga = fg->getGrupo(ordenGrupos[j]);
            Grupo* gb = fg->getGrupo(ordenGrupos[j+1]);
            if (ga && gb && mejorTercero(gb, ga))
                swap(ordenGrupos[j], ordenGrupos[j+1]);
        }
    }

    Equipo* terceros[8]; int nT = 0;
    int grupoDelTercero[8];
    for (int k = 0; k < cantG && nT < 8; k++) {
        iteraciones++;
        Grupo* gt = fg->getGrupo(ordenGrupos[k]);
        if (!gt) continue;
        Equipo* t3 = gt->obtenerClasificados()[2];
        if (t3 && !yaEnR16(t3)) {
            terceros[nT] = t3;
            grupoDelTercero[nT] = ordenGrupos[k];
            nT++;
        }
    }

    // Enfrentar terceros con rivales disponibles
    for (int t = 0; t < nT; t++) {
        iteraciones++;
        Equipo* t3 = terceros[t];
        if (yaEnR16(t3)) continue;
        Equipo* rival = nullptr;
        for (int r = 0; r < cantG && !rival; r++) {
            iteraciones++;
            if (r == grupoDelTercero[t]) continue;
            Grupo* gr = fg->getGrupo(r);
            if (!gr) continue;
            Equipo* r1 = gr->obtenerClasificados()[0];
            if (r1 && !yaEnR16(r1)) { rival = r1; }
        }
        for (int r = 0; r < cantG && !rival; r++) {
            iteraciones++;
            if (r == grupoDelTercero[t]) continue;
            Grupo* gr = fg->getGrupo(r);
            if (!gr) continue;
            Equipo* r2 = gr->obtenerClasificados()[1];
            if (r2 && !yaEnR16(r2)) { rival = r2; }
        }
        for (int u = 0; u < nT && !rival; u++) {
            iteraciones++;
            if (u == t) continue;
            if (grupoDelTercero[u] == grupoDelTercero[t]) continue;
            if (!yaEnR16(terceros[u])) { rival = terceros[u]; }
        }
        if (t3 && rival) {
            r16->agregarPartido(new Partido("2026-07-06","00:00","nombreSede",t3,rival));
            registrar(t3); registrar(rival);
        }
    }
}

// ============================================================
// Req IV: generarEstadisticas
// ============================================================
void Mundial::generarEstadisticas() {
    iteraciones = 0;
    cout << "\n========== ESTADISTICAS FINALES ==========\n";

    Equipo* campeon    = getCampeon();
    Equipo* subcampeon = nullptr;
    Equipo* tercero    = nullptr;
    Equipo* cuarto     = nullptr;

    for (int i = 0; i < cantFases; i++) {
        iteraciones++;
        if (fases[i] && fases[i]->getNombre() == "Final") {
            Partido* pf = fases[i]->getPartido(0);
            if (pf) {
                Equipo* e1 = pf->getEquipo1();
                Equipo* e2 = pf->getEquipo2();
                subcampeon = (campeon == e1) ? e2 : e1;
            }
        }
        if (fases[i] && fases[i]->getNombre() == "Tercer Puesto") {
            tercero = fases[i]->getGanadorPartido(0);
            Partido* p3 = fases[i]->getPartido(0);
            if (p3) {
                Equipo* e1 = p3->getEquipo1();
                Equipo* e2 = p3->getEquipo2();
                cuarto = (tercero == e1) ? e2 : e1;
            }
        }
    }

    cout << "\n--- Ranking Final ---\n";
    cout << "1er lugar: " << (campeon    ? campeon->getPais()    : "N/A") << "\n";
    cout << "2do lugar: " << (subcampeon ? subcampeon->getPais() : "N/A") << "\n";
    cout << "3er lugar: " << (tercero    ? tercero->getPais()    : "N/A") << "\n";
    cout << "4to lugar: " << (cuarto     ? cuarto->getPais()     : "N/A") << "\n";

    cout << "\n--- Maximo Goleador del Campeon (" << (campeon ? campeon->getPais() : "N/A") << ") ---\n";
    if (campeon) {
        Jugador* maxJug = nullptr; int maxG = -1;
        for (int j = 0; j < campeon->getTamanoPlantilla(); j++) {
            iteraciones++;
            Jugador* jug = campeon->getJugador(j);
            if (jug && jug->getGoles() > maxG) { maxG = jug->getGoles(); maxJug = jug; }
        }
        if (maxJug)
            cout << maxJug->getNombre() << " " << maxJug->getApellido()
                 << " (#" << maxJug->getNumeroCamiseta() << ") - " << maxG << " goles\n";
    }

    cout << "\n--- Top 3 Goleadores del Torneo ---\n";
    Jugador* top[3]   = {nullptr, nullptr, nullptr};
    int      topG[3]  = {-1, -1, -1};
    Equipo*  topEq[3] = {nullptr, nullptr, nullptr};
    for (int e = 0; e < cantEquipos; e++) {
        iteraciones++;
        if (!equipos[e]) continue;
        for (int j = 0; j < equipos[e]->getTamanoPlantilla(); j++) {
            iteraciones++;
            Jugador* jug = equipos[e]->getJugador(j);
            if (!jug) continue;
            int g = jug->getGoles();
            for (int k = 0; k < 3; k++) {
                iteraciones++;
                if (g > topG[k]) {
                    for (int m = 2; m > k; m--) { top[m]=top[m-1]; topG[m]=topG[m-1]; topEq[m]=topEq[m-1]; }
                    top[k]=jug; topG[k]=g; topEq[k]=equipos[e];
                    break;
                }
            }
        }
    }
    for (int k = 0; k < 3; k++) {
        iteraciones++;
        if (top[k])
            cout << (k+1) << ". " << top[k]->getNombre() << " " << top[k]->getApellido()
                 << " (#" << top[k]->getNumeroCamiseta() << ") - "
                 << topEq[k]->getPais() << " - " << topG[k] << " goles\n";
    }

    cout << "\n--- Equipo con mas goles historicos (post-copa) ---\n";
    Equipo* masGoles = getEquipoMasGoles();
    if (masGoles)
        cout << masGoles->getPais() << " con " << masGoles->getGFA() << " goles a favor historicos\n";

    const char* etapas[3]    = {"Dieciseisavos (R16)", "Octavos (R8)", "Cuartos (QF)"};
    string confNombres[6]    = {"UEFA","CONMEBOL","Concacaf","CAF","AFC","OFC"};
    for (int e = 0; e < 3; e++) {
        iteraciones++;
        int confCount[6] = {0};
        for (int f = 0; f < cantFases; f++) {
            iteraciones++;
            if (!fases[f] || fases[f]->getNombre() != etapas[e]) continue;
            for (int p = 0; p < fases[f]->getCantPartidos(); p++) {
                iteraciones++;
                Partido* par = fases[f]->getPartido(p);
                if (!par) continue;
                Equipo* eq[2] = {par->getEquipo1(), par->getEquipo2()};
                for (int q = 0; q < 2; q++) {
                    iteraciones++;
                    if (!eq[q]) continue;
                    for (int c = 0; c < 6; c++) {
                        iteraciones++;
                        if (eq[q]->getConfederacion() == confNombres[c]) { confCount[c]++; break; }
                    }
                }
            }
        }
        string maxConf = ""; int maxC = 0;
        for (int c = 0; c < 6; c++) { iteraciones++; if (confCount[c] > maxC) { maxC=confCount[c]; maxConf=confNombres[c]; } }
        cout << "\nConfederacion con mayor presencia en " << etapas[e]
             << ": " << maxConf << " (" << maxC << " equipos)\n";
    }

    imprimirMetricas("generarEstadisticas");
}

// ============================================================
// getCampeon / getEquipoMasGoles
// ============================================================
Equipo* Mundial::getCampeon() {
    for (int i = 0; i < cantFases; i++)
        if (fases[i] && fases[i]->getNombre() == "Final")
            return fases[i]->getGanadorPartido(0);
    return nullptr;
}

Equipo* Mundial::getEquipoMasGoles() {
    Equipo* mejor = nullptr; int max = -1;
    for (int i = 0; i < cantEquipos; i++) {
        iteraciones++;
        if (equipos[i] && equipos[i]->getGFA() > max) { max = equipos[i]->getGFA(); mejor = equipos[i]; }
    }
    return mejor;
}
