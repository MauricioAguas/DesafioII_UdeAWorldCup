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
static long long bytesEstJug() { return sizeof(EstadisticasJugador); }
static long long bytesJugador(const Jugador* j) {
    if (!j) return 0;
    return sizeof(Jugador) + bytesEstJug();
}
static long long bytesEstEq() { return sizeof(EstadisticasEquipo); }
static long long bytesEquipo(const Equipo* e) {
    if (!e) return 0;
    return sizeof(Equipo) + bytesEstEq();
}
static long long bytesResultado(const Resultado* r) {
    if (!r) return 0;
    return sizeof(Resultado);
}
static long long bytesPartido(const Partido* p) {
    if (!p) return 0;
    return sizeof(Partido) + bytesResultado(p->getResultado());
}
static long long bytesGrupo(const Grupo* g) {
    if (!g) return 0;
    return sizeof(Grupo);
}
static long long bytesFase(const Fase* f) {
    if (!f) return 0;
    long long b = sizeof(Fase);
    for (int p = 0; p < f->getCantPartidos(); p++)
        b += bytesPartido(f->getPartido(p));
    return b;
}

long long Mundial::calcularMemoria() {
    long long total = sizeof(Mundial);
    for (int i = 0; i < cantEquipos; i++) {
        if (!equipos[i]) continue;
        total += bytesEquipo(equipos[i]);
        for (int j = 0; j < equipos[i]->getTamanoPlantilla(); j++)
            total += bytesJugador(equipos[i]->getJugador(j));
    }
    for (int g = 0; g < 12; g++) total += bytesGrupo(grupos[g]);
    for (int f = 0; f < cantFases; f++) total += bytesFase(fases[f]);
    return total;
}

void Mundial::imprimirMetricas(const string& nombreFunc) {
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
    getline(archivo, linea); iteraciones++;
    getline(archivo, linea); iteraciones++;
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
        e->cargarJugadores(); iteraciones += 26;
        equipos[cantEquipos++] = e;
    }
    archivo.close();
    cout << "[Mundial] " << cantEquipos << " equipos cargados desde CSV.\n";
    imprimirMetricas("cargarEquipos");
}

// ============================================================
// helpers estaticos para conformarGrupos
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

    for (int i = 11; i > 0; i--) {
        int j = rand() % (i+1);
        swap(bombos[0][i], bombos[0][j]);
        iteraciones++;
    }
    for (int g = 0; g < 12; g++) { grupos[g]->agregarEquipo(equipos[bombos[0][g]]); iteraciones++; }

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
            // FIX: fallback respeta espacio y unicidad, pero relaja restriccion
            // de confederacion cuando matematicamente es imposible cumplirla
            // (ocurre con 17 UEFA en 12 grupos, limite 2/grupo: 31 no-UEFA > 24 slots)
            if (nd == 0) {
                for (int g = 0; g < 12; g++) {
                    iteraciones++;
                    if (grupos[g]->getCantEquipos() < 4
                        && !equipoYaEnGrupo(grupos[g], eq)
                        && puedeEntrar(grupos[g], eq))
                        disponibles[nd++] = g;
                }
                // Segunda relajacion: solo espacio y unicidad
                if (nd == 0) {
                    for (int g = 0; g < 12; g++) {
                        iteraciones++;
                        if (grupos[g]->getCantEquipos() < 4
                            && !equipoYaEnGrupo(grupos[g], eq))
                            disponibles[nd++] = g;
                    }
                }
            }
            if (nd == 0) {
                cerr << "[Sorteo] Sin grupo disponible para " << eq->getPais() << "\n";
                continue;
            }
            int elegido = disponibles[rand() % nd];
            grupos[elegido]->agregarEquipo(eq);
        }
    }

    cout << "[Mundial] Grupos conformados por bombos:\n";
    for (int g = 0; g < 12; g++) {
        cout << "Grupo " << letras[g] << ": ";
        for (int e = 0; e < grupos[g]->getCantEquipos(); e++) {
            cout << grupos[g]->getEquipo(e)->getPais()
                 << " (" << grupos[g]->getEquipo(e)->getConfederacion() << ")  ";
        }
        cout << "\n";
    }
    imprimirMetricas("conformarGrupos");
}

// ============================================================
// SCHEDULER GLOBAL de fechas para la fase de grupos
// Reglas del enunciado:
//   - Max 4 partidos por dia en todo el mundial
//   - Un equipo no puede jugar dos partidos en un lapso de 3 dias
//   - Inicio: 2026-06-20  (dia base = 0)
//   - Ventana: 19 dias (dias 0..18)  <-- FIX: limite estricto dia 18
// ============================================================
static string sumarDias(int diaOffset) {
    int anio = 2026, mes = 6, dia = 20;
    int diasMes[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    dia += diaOffset;
    while (dia > diasMes[mes]) { dia -= diasMes[mes]; mes++; if(mes>12){mes=1;anio++;} }
    string f = to_string(anio) + "-"
             + (mes<10?"0":"") + to_string(mes) + "-"
             + (dia<10?"0":"") + to_string(dia);
    return f;
}

void Mundial::schedulerFaseGrupos() {
    int e1idx[6] = {0,0,0,1,1,2};
    int e2idx[6] = {1,2,3,2,3,3};

    // FIX: ventana estricta de 19 dias (0..18), arreglo dimensionado a 19
    const int MAX_DIA = 18;
    int partidosPorDia[19] = {0};

    int ultimoDia[48];
    for (int i = 0; i < 48; i++) ultimoDia[i] = -99;

    auto idxEquipo = [&](Grupo* g, int posEnGrupo) -> int {
        Equipo* eq = g->getEquipo(posEnGrupo);
        for (int i = 0; i < cantEquipos; i++)
            if (equipos[i] == eq) return i;
        return -1;
    };

    for (int g = 0; g < 12; g++) {
        iteraciones++;
        Grupo* gr = grupos[g];
        if (!gr) continue;

        string fechas[6];
        for (int p = 0; p < 6; p++) {
            iteraciones++;
            int ie1 = idxEquipo(gr, e1idx[p]);
            int ie2 = idxEquipo(gr, e2idx[p]);

            int dia = 0;
            bool encontrado = false;
            while (dia <= MAX_DIA && !encontrado) {
                iteraciones++;
                bool diaLibre    = (partidosPorDia[dia] < 4);
                bool eq1descansa = (ie1 < 0 || ultimoDia[ie1] < 0 || (dia - ultimoDia[ie1]) > 3);
                bool eq2descansa = (ie2 < 0 || ultimoDia[ie2] < 0 || (dia - ultimoDia[ie2]) > 3);
                if (diaLibre && eq1descansa && eq2descansa) {
                    encontrado = true;
                } else {
                    dia++;
                }
            }

            if (!encontrado) {
                int mejorDia = -1, menorViolacion = 9999;
                for (int d = 0; d <= MAX_DIA; d++) {
                    iteraciones++;
                    if (partidosPorDia[d] >= 4) continue;
                    int v1 = (ie1 >= 0 && ultimoDia[ie1] >= 0) ? (3 - (d - ultimoDia[ie1])) : 0;
                    int v2 = (ie2 >= 0 && ultimoDia[ie2] >= 0) ? (3 - (d - ultimoDia[ie2])) : 0;
                    int violacion = (v1 > 0 ? v1 : 0) + (v2 > 0 ? v2 : 0);
                    if (violacion < menorViolacion) {
                        menorViolacion = violacion;
                        mejorDia = d;
                    }
                }
                dia = (mejorDia >= 0) ? mejorDia : MAX_DIA;
            }

            fechas[p] = sumarDias(dia);
            partidosPorDia[dia]++;
            if (ie1 >= 0) ultimoDia[ie1] = dia;
            if (ie2 >= 0) ultimoDia[ie2] = dia;
        }
        gr->configurarPartidosConFechas(fechas);
    }
}

// ============================================================
// helper: devuelve el indice de grupo (0..11) al que pertenece
// un equipo en la fase de grupos. Retorna -1 si no lo encuentra.
// ============================================================
static int grupoDeEquipo(Fase* fg, Equipo* eq) {
    for (int g = 0; g < fg->getCantGrupos(); g++) {
        Grupo* gr = fg->getGrupo(g);
        if (!gr) continue;
        for (int e = 0; e < gr->getCantEquipos(); e++)
            if (gr->getEquipo(e) == eq) return g;
    }
    return -1;
}

// ============================================================
// Req III: simularTorneo
// ============================================================
void Mundial::simularTorneo() {
    iteraciones = 0;

    Fase* faseGrupos = new Fase("Fase de Grupos", "grupos", "2026-06-20");
    for (int g = 0; g < 12; g++) { faseGrupos->agregarGrupo(grupos[g]); iteraciones++; }

    schedulerFaseGrupos();            iteraciones += 72;

    faseGrupos->simularPartidos();    iteraciones += 72;
    faseGrupos->generarTablas();      iteraciones += 48;
    faseGrupos->imprimirResultados();
    fases[cantFases++] = faseGrupos;

    // FIX: fecha fija 2026-07-10 para R16 segun enunciado
    Fase* faseR16 = new Fase("Dieciseisavos (R16)", "eliminacion", "2026-07-10");
    armarR16(faseGrupos, faseR16);

    cout << "\n--- Fixture R16 (antes de simular) ---\n";
    for (int i = 0; i < faseR16->getCantPartidos(); i++) {
        iteraciones++;
        Partido* p = faseR16->getPartido(i);
        if (p) cout << "  " << p->getEquipo1()->getPais()
                    << " vs " << p->getEquipo2()->getPais() << "\n";
    }

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

    for (int g = 0; g + 1 < cantG; g += 2) {
        iteraciones++;
        Grupo* ga = fg->getGrupo(g);
        Grupo* gb = fg->getGrupo(g+1);
        if (!ga || !gb) continue;
        // FIX: guard — verificar que cada grupo tenga al menos 2 clasificados
        if (ga->getCantEquipos() < 2 || gb->getCantEquipos() < 2) continue;

        Equipo** clA = ga->obtenerClasificados();
        Equipo** clB = gb->obtenerClasificados();
        Equipo* g1a = clA[0];
        Equipo* g2a = clA[1];
        Equipo* g1b = clB[0];
        Equipo* g2b = clB[1];

        bool cruceOk1 = (g1a && g2b && grupoDeEquipo(fg,g1a) != grupoDeEquipo(fg,g2b));
        bool cruceOk2 = (g1b && g2a && grupoDeEquipo(fg,g1b) != grupoDeEquipo(fg,g2a));

        if (!cruceOk1 && g1a && g2a &&
            grupoDeEquipo(fg,g1a) != grupoDeEquipo(fg,g2a) &&
            !yaEnR16(g1a) && !yaEnR16(g2a)) {
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g1a,g2a));
            registrar(g1a); registrar(g2a);
        } else if (g1a && g2b && !yaEnR16(g1a) && !yaEnR16(g2b)) {
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g1a,g2b));
            registrar(g1a); registrar(g2b);
        }

        if (!cruceOk2 && g1b && g2b &&
            grupoDeEquipo(fg,g1b) != grupoDeEquipo(fg,g2b) &&
            !yaEnR16(g1b) && !yaEnR16(g2b)) {
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g1b,g2b));
            registrar(g1b); registrar(g2b);
        } else if (g1b && g2a && !yaEnR16(g1b) && !yaEnR16(g2a)) {
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",g1b,g2a));
            registrar(g1b); registrar(g2a);
        }
    }

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
        // FIX: guard — verificar que el grupo tenga al menos 3 equipos clasificados
        if (gt->getCantEquipos() < 3) continue;
        Equipo** clT = gt->obtenerClasificados();
        Equipo* t3 = clT[2];
        if (t3 && !yaEnR16(t3)) {
            terceros[nT] = t3;
            grupoDelTercero[nT] = ordenGrupos[k];
            nT++;
        }
    }

    for (int t = 0; t < nT; t++) {
        iteraciones++;
        Equipo* t3 = terceros[t];
        if (yaEnR16(t3)) continue;
        Equipo* rival = nullptr;
        for (int r = 0; r < cantG && !rival; r++) {
            iteraciones++;
            if (r == grupoDelTercero[t]) continue;
            Grupo* gr = fg->getGrupo(r);
            if (!gr || gr->getCantEquipos() < 1) continue;
            Equipo* r1 = gr->obtenerClasificados()[0];
            if (r1 && !yaEnR16(r1) && grupoDeEquipo(fg,r1) != grupoDelTercero[t])
                { rival = r1; }
        }
        for (int r = 0; r < cantG && !rival; r++) {
            iteraciones++;
            if (r == grupoDelTercero[t]) continue;
            Grupo* gr = fg->getGrupo(r);
            if (!gr || gr->getCantEquipos() < 2) continue;
            Equipo* r2 = gr->obtenerClasificados()[1];
            if (r2 && !yaEnR16(r2) && grupoDeEquipo(fg,r2) != grupoDelTercero[t])
                { rival = r2; }
        }
        for (int u = 0; u < nT && !rival; u++) {
            iteraciones++;
            if (u == t) continue;
            if (grupoDelTercero[u] == grupoDelTercero[t]) continue;
            if (!yaEnR16(terceros[u])) { rival = terceros[u]; }
        }
        if (t3 && rival) {
            r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",t3,rival));
            registrar(t3); registrar(rival);
        }
    }
}

// ============================================================
// Req IV: generarEstadisticas
// ============================================================
void Mundial::generarEstadisticas() {
    iteraciones = 0;
    cout << "\n=== Generando estadisticas finales... ===\n";
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
