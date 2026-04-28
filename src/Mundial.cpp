#include "../hds/Mundial.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============================================================
// HELPERS PRIVADOS PARA CALCULO DE MEMORIA
// Estas funciones calculan cuántos bytes ocupa cada objeto en
// memoria, sumando el tamaño de la estructura base más sus
// dependencias (jugadores, partidos, etc.).
// ============================================================
static long long bytesEstJug() { return sizeof(EstadisticasJugador); }
static long long bytesJugador(const Jugador* j) {
    if (!j) return 0;
    // Tamaño del jugador + sus estadísticas históricas
    return sizeof(Jugador) + bytesEstJug();
}
static long long bytesEstEq() { return sizeof(EstadisticasEquipo); }
static long long bytesEquipo(const Equipo* e) {
    if (!e) return 0;
    // Tamaño del equipo + sus estadísticas históricas (los jugadores se cuentan aparte)
    return sizeof(Equipo) + bytesEstEq();
}
static long long bytesResultado(const Resultado* r) {
    if (!r) return 0;
    return sizeof(Resultado);
}
static long long bytesPartido(const Partido* p) {
    if (!p) return 0;
    // Tamaño del partido + el resultado asociado
    return sizeof(Partido) + bytesResultado(p->getResultado());
}
static long long bytesGrupo(const Grupo* g) {
    if (!g) return 0;
    return sizeof(Grupo);
}
static long long bytesFase(const Fase* f) {
    if (!f) return 0;
    long long b = sizeof(Fase);
    // Sumamos el tamaño de cada partido dentro de la fase
    for (int p = 0; p < f->getCantPartidos(); p++)
        b += bytesPartido(f->getPartido(p));
    return b;
}

// Calcula la memoria total consumida por el sistema en el momento de la llamada.
// Recorre equipos, grupos y fases acumulando el tamaño de cada objeto.
long long Mundial::calcularMemoria() {
    long long total = sizeof(Mundial);
    for (int i = 0; i < cantEquipos; i++) {
        if (!equipos[i]) continue;
        total += bytesEquipo(equipos[i]);
        // Cada jugador de la plantilla también ocupa memoria
        for (int j = 0; j < equipos[i]->getTamanoPlantilla(); j++)
            total += bytesJugador(equipos[i]->getJugador(j));
    }
    for (int g = 0; g < 12; g++) total += bytesGrupo(grupos[g]);
    for (int f = 0; f < cantFases; f++) total += bytesFase(fases[f]);
    return total;
}

// Imprime en consola las dos métricas de eficiencia requeridas (Req V):
//   - Iteraciones acumuladas durante la funcionalidad
//   - Memoria total consumida en ese instante
void Mundial::imprimirMetricas(const string& nombreFunc) {
    cout << "\n[Metricas - " << nombreFunc << "]\n";
    cout << "  Iteraciones ejecutadas : " << iteraciones << "\n";
    cout << "  Memoria consumida      : " << calcularMemoria() << " bytes\n";
}

// ============================================================
// CONSTRUCTORES / DESTRUCTOR
// ============================================================

// Constructor por defecto: inicializa todos los punteros a nullptr
// y los contadores a 0. Siempre se invoca desde el constructor con parámetros.
Mundial::Mundial() {
    anio = cantEquipos = cantFases = 0;
    iteraciones = 0;
    for (int i = 0; i < 48; i++) equipos[i] = nullptr;
    for (int i = 0; i < 12; i++) grupos[i]  = nullptr;
    for (int i = 0; i < 8;  i++) fases[i]   = nullptr;
}

// Constructor principal: recibe el año del mundial y la cantidad de equipos.
// Llama al constructor por defecto con "Mundial()" para reutilizar la inicialización.
// srand inicializa la semilla aleatoria una sola vez con el tiempo actual.
Mundial::Mundial(int anio, int cantEquipos) : Mundial() {
    this->anio        = anio;
    this->cantEquipos = cantEquipos;
    srand(time(nullptr));
}

// Destructor: libera toda la memoria dinámica reservada durante la ejecución.
// Es fundamental para evitar memory leaks.
Mundial::~Mundial() {
    for (int i = 0; i < cantEquipos; i++) if (equipos[i]) delete equipos[i];
    for (int i = 0; i < 12; i++)          if (grupos[i])  delete grupos[i];
    for (int i = 0; i < cantFases; i++)   if (fases[i])   delete fases[i];
}

// ============================================================
// Req I: cargarEquipos
// Lee el archivo CSV con los datos históricos de las 48 selecciones
// y construye dinámicamente los objetos Equipo con sus jugadores.
// ============================================================
void Mundial::cargarEquipos() {
    iteraciones = 0;
    ifstream archivo("data/selecciones_clasificadas_mundial.csv");
    if (!archivo.is_open()) {
        cerr << "[Error] No se pudo abrir data/selecciones_clasificadas_mundial.csv\n";
        return;
    }
    string linea;
    // Saltamos las dos primeras líneas (encabezados del CSV)
    getline(archivo, linea); iteraciones++;
    getline(archivo, linea); iteraciones++;
    cantEquipos = 0;

    while (getline(archivo, linea) && cantEquipos < 48) {
        iteraciones++;
        if (linea.empty()) continue;
        // Eliminar '\r' en sistemas Windows (CRLF -> LF)
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        istringstream ss(linea);
        string rankStr, pais, DT, federacion, conf, gfStr, gcStr, ganStr, empStr, perStr;
        // Cada getline sobre istringstream extrae un campo delimitado por ','
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

        // Crear el equipo con datos básicos y asignar estadísticas históricas
        Equipo* e = new Equipo(pais, conf, ranking, DT);
        e->setEstadisticasIniciales(gf, gc, gan, emp, per); iteraciones++;
        // cargarJugadores genera los 26 jugadores con nombre/apellido numerados
        // y reparte los goles históricos uniformemente entre ellos
        e->cargarJugadores(); iteraciones += 26;
        equipos[cantEquipos++] = e;
    }
    archivo.close();
    cout << "[Mundial] " << cantEquipos << " equipos cargados desde CSV.\n";
    imprimirMetricas("cargarEquipos");
}

// ============================================================
// helpers estáticos para conformarGrupos
// Funciones auxiliares que verifican restricciones de confederación
// al momento de asignar equipos a grupos.
// ============================================================

// Cuenta cuántos equipos de una confederación dada ya están en el grupo g
static int contarConf(Grupo* g, const string& conf) {
    int c = 0;
    for (int i = 0; i < g->getCantEquipos(); i++)
        if (g->getEquipo(i) && g->getEquipo(i)->getConfederacion() == conf) c++;
    return c;
}

// Verifica si el equipo 'e' puede entrar al grupo 'g' respetando la restricción:
//   - UEFA: máximo 2 equipos por grupo
//   - Cualquier otra confederación: máximo 1 equipo por grupo
static bool puedeEntrar(Grupo* g, Equipo* e) {
    string conf = e->getConfederacion();
    int limite  = (conf == "UEFA") ? 2 : 1;
    return contarConf(g, conf) < limite;
}

// Verifica si el equipo ya fue asignado al grupo (evitar duplicados)
static bool equipoYaEnGrupo(Grupo* g, Equipo* e) {
    for (int i = 0; i < g->getCantEquipos(); i++)
        if (g->getEquipo(i) == e) return true;
    return false;
}

// ============================================================
// Req II: conformarGrupos
// Realiza el sorteo del mundial siguiendo el procedimiento oficial:
//   1. Ordena los equipos por ranking FIFA y los distribuye en 4 bombos de 12
//   2. EE.UU. va directo al bombo 1 (país anfitrión)
//   3. Mezcla aleatoriamente cada bombo (Fisher-Yates)
//   4. Asigna un equipo por bombo a cada grupo respetando restricciones de confederación
// ============================================================
void Mundial::conformarGrupos() {
    iteraciones = 0;
    char letras[12] = {'A','B','C','D','E','F','G','H','I','J','K','L'};
    // Crear los 12 grupos identificados por letra
    for (int g = 0; g < 12; g++) { grupos[g] = new Grupo(letras[g]); iteraciones++; }

    // Crear índices para ordenar equipos sin mover los punteros originales
    int idx[48];
    for (int i = 0; i < cantEquipos; i++) { idx[i] = i; iteraciones++; }

    // Ordenar índices por ranking FIFA usando burbuja (menor ranking = mejor posición)
    // Se usa burbuja por ser una estructura de datos propia, sin STL
    for (int i = 0; i < cantEquipos - 1; i++) {
        for (int j = 0; j < cantEquipos - 1 - i; j++) {
            iteraciones++;
            if (equipos[idx[j]]->getRanking() > equipos[idx[j+1]]->getRanking()) {
                // Intercambio manual
                int tmp = idx[j]; idx[j] = idx[j+1]; idx[j+1] = tmp;
            }
        }
    }

    // Distribuir los equipos ordenados en 4 bombos de 12 (bombo 1 = mejor ranking)
    int bombos[4][12];
    for (int b = 0; b < 4; b++)
        for (int i = 0; i < 12; i++) { bombos[b][i] = idx[b*12+i]; iteraciones++; }

    // Mezcla aleatoria del bombo 1 con algoritmo Fisher-Yates
    // Fisher-Yates garantiza una permutación uniforme y sin repeticiones
    for (int i = 11; i > 0; i--) {
        int j = rand() % (i+1);
        // Intercambio manual
        int tmp = bombos[0][i]; bombos[0][i] = bombos[0][j]; bombos[0][j] = tmp;
        iteraciones++;
    }
    // Asignar cabezas de serie (bombo 1) directamente: un equipo por grupo
    for (int g = 0; g < 12; g++) { grupos[g]->agregarEquipo(equipos[bombos[0][g]]); iteraciones++; }

    // Para los bombos 2, 3 y 4: mezclar y asignar respetando restricciones de confederación
    for (int b = 1; b < 4; b++) {
        // Mezcla Fisher-Yates para el bombo actual
        for (int i = 11; i > 0; i--) {
            int j = rand() % (i+1);
            // Intercambio manual
            int tmp = bombos[b][i]; bombos[b][i] = bombos[b][j]; bombos[b][j] = tmp;
            iteraciones++;
        }
        for (int e = 0; e < 12; e++) {
            iteraciones++;
            Equipo* eq = equipos[bombos[b][e]];
            // Buscar grupos disponibles que cumplan las restricciones de confederación
            int disponibles[12]; int nd = 0;
            for (int g = 0; g < 12; g++) {
                iteraciones++;
                if (grupos[g]->getCantEquipos() < 4
                    && !equipoYaEnGrupo(grupos[g], eq)
                    && puedeEntrar(grupos[g], eq))
                    disponibles[nd++] = g;
            }
            // Si no hay grupos que cumplan todas las restricciones, relajamos la de confederación
            // (fallback para evitar bloqueos en el sorteo)
            if (nd == 0) {
                for (int g = 0; g < 12; g++) {
                    iteraciones++;
                    if (grupos[g]->getCantEquipos() < 4
                        && !equipoYaEnGrupo(grupos[g], eq))
                        disponibles[nd++] = g;
                }
            }
            if (nd == 0) {
                cerr << "[Sorteo] Sin grupo disponible para " << eq->getPais() << "\n";
                continue;
            }
            // Elegir aleatoriamente entre los grupos disponibles
            int elegido = disponibles[rand() % nd];
            grupos[elegido]->agregarEquipo(eq);
        }
    }

    // Imprimir los grupos conformados con confederación de cada equipo
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
// Genera la fecha de cada partido a partir del día de inicio (20-jun-2026)
// sumando un offset en días.
// ============================================================

// Convierte un offset de días desde el 20/06/2026 en una fecha "YYYY-MM-DD"
static string sumarDias(int diaOffset) {
    int anio = 2026, mes = 6, dia = 20;
    int diasMes[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    dia += diaOffset;
    // Avanzar mes si el día supera los días del mes actual
    while (dia > diasMes[mes]) { dia -= diasMes[mes]; mes++; if(mes>12){mes=1;anio++;} }
    string f = to_string(anio) + "-"
             + (mes<10?"0":"") + to_string(mes) + "-"
             + (dia<10?"0":"") + to_string(dia);
    return f;
}

// Asigna fechas a los 6 partidos de cada grupo respetando:
//   - Máximo 4 partidos por día en todo el torneo
//   - Ningún equipo juega con menos de 3 días de diferencia entre sus partidos
// Usa un arreglo de días disponibles y un registro del último día jugado por equipo.
void Mundial::schedulerFaseGrupos() {
    // e1idx y e2idx definen las 6 combinaciones de partidos dentro de un grupo de 4
    // (todos contra todos una vez): 0vs1, 0vs2, 0vs3, 1vs2, 1vs3, 2vs3
    int e1idx[6] = {0,0,0,1,1,2};
    int e2idx[6] = {1,2,3,2,3,3};

    const int MAX_DIA = 18; // La fase de grupos dura 19 días (día 0 al 18)
    int partidosPorDia[19] = {0}; // Contador de partidos por día global

    // Registro del último día que jugó cada equipo (índice global)
    int ultimoDia[48];
    for (int i = 0; i < 48; i++) ultimoDia[i] = -99;

    // Lambda para obtener el índice global de un equipo dentro del arreglo equipos[]
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

            // Buscar el primer día que cumpla todas las restricciones
            int dia = 0;
            bool encontrado = false;
            while (dia <= MAX_DIA && !encontrado) {
                iteraciones++;
                bool diaLibre    = (partidosPorDia[dia] < 4);
                // El equipo debe haber descansado al menos 3 días desde su último partido
                bool eq1descansa = (ie1 < 0 || ultimoDia[ie1] < 0 || (dia - ultimoDia[ie1]) >= 3);
                bool eq2descansa = (ie2 < 0 || ultimoDia[ie2] < 0 || (dia - ultimoDia[ie2]) >= 3);
                if (diaLibre && eq1descansa && eq2descansa) {
                    encontrado = true;
                } else {
                    dia++;
                }
            }

            // Si no se encontró día ideal, elegir el que tenga menos violaciones
            // (fallback para evitar bloqueos en el scheduling)
            if (!encontrado) {
                int mejorDia = 0, menorViolacion = 9999;
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
                dia = mejorDia;
            }

            fechas[p] = sumarDias(dia);
            partidosPorDia[dia]++;
            // Registrar el día jugado para cada equipo
            if (ie1 >= 0) ultimoDia[ie1] = dia;
            if (ie2 >= 0) ultimoDia[ie2] = dia;
        }
        gr->configurarPartidosConFechas(fechas);
    }
}

// ============================================================
// Req III: simularTorneo
// Orquesta la simulación completa del torneo en orden:
//   Fase de grupos -> R16 -> R8 -> QF -> SF -> 3er puesto -> Final
// Cada fase crea sus partidos, los simula y actualiza estadísticas.
// ============================================================
void Mundial::simularTorneo() {
    iteraciones = 0;

    // --- FASE DE GRUPOS ---
    Fase* faseGrupos = new Fase("Fase de Grupos", "grupos", "2026-06-20");
    for (int g = 0; g < 12; g++) { faseGrupos->agregarGrupo(grupos[g]); iteraciones++; }

    // Asignar fechas a los 72 partidos de grupos respetando restricciones del calendario
    schedulerFaseGrupos();            iteraciones += 72;

    // Simular los 72 partidos (fórmula λ de Poisson para goles, estadísticas de jugadores)
    faseGrupos->simularPartidos();    iteraciones += 72;
    // Calcular tablas de clasificación (puntos, diferencia de goles, goles a favor)
    faseGrupos->generarTablas();      iteraciones += 48;
    faseGrupos->imprimirResultados();
    fases[cantFases++] = faseGrupos;

    // --- DIECISEISAVOS (R16): 16 partidos ---
    Fase* faseR16 = new Fase("Dieciseisavos (R16)", "eliminacion", "2026-07-10");
    // armarR16 clasifica 12 primeros + 12 segundos + 8 mejores terceros
    // y configura los cruces sin repetir grupos de la fase anterior
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

    // --- OCTAVOS (R8): 8 partidos ---
    // Los ganadores de R16 se emparejan consecutivamente: ganador partido 0 vs ganador partido 1, etc.
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

    // --- CUARTOS (QF): 4 partidos ---
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

    // --- SEMIFINALES (SF): 2 partidos ---
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

    // --- TERCER PUESTO: los perdedores de semifinales ---
    // Identificar los perdedores de cada semifinal comparando el ganador con los participantes
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

    // --- FINAL: los dos ganadores de semifinales ---
    Fase* faseFinal = new Fase("Final", "eliminacion", "2026-07-10");
    if (sf1g && sf2g)
        faseFinal->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",sf1g,sf2g));
    faseFinal->simularPartidos(); iteraciones++;
    faseFinal->imprimirResultados();
    fases[cantFases++] = faseFinal;

    imprimirMetricas("simularTorneo");
}

// ============================================================
// helper: comparar terceros por pts > dg > gf
// Usado para ordenar los 12 terceros y elegir los 8 mejores
// ============================================================
static bool mejorTercero(Grupo* a, int idxA, Grupo* b, int idxB) {
    if (a->getPuntos(idxA) != b->getPuntos(idxB))
        return a->getPuntos(idxA) > b->getPuntos(idxB);
    if (a->getDifGoles(idxA) != b->getDifGoles(idxB))
        return a->getDifGoles(idxA) > b->getDifGoles(idxB);
    return a->getGolesFavor(idxA) > b->getGolesFavor(idxB);
}

// ============================================================
// armarR16
// Clasifica los 32 equipos que pasan a R16 y configura sus cruces:
//   - 12 primeros de grupo (cabezas de serie)
//   - 12 segundos de grupo
//   - 8 mejores terceros (elegidos por pts > dif. goles > goles a favor)
// Regla principal: ningún cruce puede enfrentar equipos del mismo grupo.
// ============================================================
void Mundial::armarR16(Fase* fg, Fase* r16) {
    int cantG = fg->getCantGrupos();

    // Arreglos para almacenar primeros, segundos y terceros de cada grupo
    Equipo* primeros[12];  int grupoPrimero[12];
    Equipo* segundos[12];  int grupoSegundo[12];
    Equipo* todosT[12];    int grupoTercero[12]; int ptsT[12]; int dgT[12]; int gfT[12];
    int nP = 0, nS = 0, nT = 0;

    // Extraer clasificados de cada grupo
    for (int g = 0; g < cantG; g++) {
        iteraciones++;
        Grupo* gr = fg->getGrupo(g);
        if (!gr || gr->getCantEquipos() < 3) continue;
        Equipo** cl = gr->obtenerClasificados(); // Retorna equipos ordenados por puntos
        primeros[nP] = cl[0]; grupoPrimero[nP++] = g;
        segundos[nS] = cl[1]; grupoSegundo[nS++] = g;
        int idx3 = -1;
        for (int i = 0; i < gr->getCantEquipos(); i++)
            if (gr->getEquipo(i) == cl[2]) { idx3 = i; break; }
        todosT[nT]    = cl[2];
        grupoTercero[nT] = g;
        ptsT[nT] = (idx3 >= 0) ? gr->getPuntos(idx3)    : 0;
        dgT[nT]  = (idx3 >= 0) ? gr->getDifGoles(idx3)  : 0;
        gfT[nT]  = (idx3 >= 0) ? gr->getGolesFavor(idx3): 0;
        nT++;
    }

    // Ordenar terceros por pts > dg > gf para identificar los 8 mejores
    // Usamos burbuja (estructura propia, sin STL)
    for (int i = 0; i < nT - 1; i++) {
        for (int j = 0; j < nT - 1 - i; j++) {
            iteraciones++;
            bool swap_needed = false;
            if (ptsT[j] < ptsT[j+1]) swap_needed = true;
            else if (ptsT[j] == ptsT[j+1] && dgT[j] < dgT[j+1]) swap_needed = true;
            else if (ptsT[j] == ptsT[j+1] && dgT[j] == dgT[j+1] && gfT[j] < gfT[j+1]) swap_needed = true;
            if (swap_needed) {
                // Intercambios manuales para los 5 arreglos paralelos (reemplaza swap() de <algorithm>)
                Equipo* tmpE = todosT[j];       todosT[j]       = todosT[j+1];       todosT[j+1]       = tmpE;
                int     tmpI = grupoTercero[j]; grupoTercero[j] = grupoTercero[j+1]; grupoTercero[j+1] = tmpI;
                int     tmpP = ptsT[j];         ptsT[j]         = ptsT[j+1];         ptsT[j+1]         = tmpP;
                int     tmpD = dgT[j];          dgT[j]          = dgT[j+1];          dgT[j+1]          = tmpD;
                int     tmpG = gfT[j];          gfT[j]          = gfT[j+1];          gfT[j+1]          = tmpG;
            }
        }
    }

    // Los primeros 8 en el arreglo ordenado son los mejores terceros
    int mejores8T[8]; int grupoDe8T[8];
    for (int i = 0; i < 8 && i < nT; i++) { mejores8T[i] = i; grupoDe8T[i] = grupoTercero[i]; iteraciones++; }

    bool primeroUsado[12] = {false};
    bool segundoUsado[12] = {false};

    // Lambda auxiliar para agregar partidos a la fase R16
    auto agregarPartido = [&](Equipo* e1, Equipo* e2) {
        r16->agregarPartido(new Partido("2026-07-10","00:00","nombreSede",e1,e2));
        iteraciones++;
    };

    // PASO 1: Enfrentar los 8 mejores terceros contra primeros de grupo distinto
    for (int t = 0; t < 8; t++) {
        iteraciones++;
        int idxT = mejores8T[t];
        Equipo* tercero = todosT[idxT];
        int gT = grupoDe8T[t];
        Equipo* rival = nullptr;
        int rivalP = -1;
        // Buscar un primero disponible que no sea del mismo grupo
        for (int p = 0; p < nP && !rival; p++) {
            iteraciones++;
            if (primeroUsado[p]) continue;
            if (grupoPrimero[p] == gT) continue; // No pueden ser del mismo grupo
            rival = primeros[p];
            rivalP = p;
        }
        if (rival && tercero) {
            agregarPartido(primeros[rivalP], tercero);
            primeroUsado[rivalP] = true;
        }
    }

    // PASO 2: Los primeros sin rival jugarán contra los peores 4 segundos
    int sinRival[12]; int nSR = 0;
    for (int p = 0; p < nP; p++) { iteraciones++; if (!primeroUsado[p]) sinRival[nSR++] = p; }

    // Ordenar segundos por rendimiento para identificar los 4 peores
    int idxSeg[12]; for (int i = 0; i < nS; i++) idxSeg[i] = i;
    for (int i = 0; i < nS - 1; i++) {
        for (int j = 0; j < nS - 1 - i; j++) {
            iteraciones++;
            Grupo* ga = fg->getGrupo(grupoSegundo[idxSeg[j]]);
            Grupo* gb = fg->getGrupo(grupoSegundo[idxSeg[j+1]]);
            if (!ga || !gb) continue;
            int iA = -1, iB = -1;
            for (int k = 0; k < ga->getCantEquipos(); k++)
                if (ga->getEquipo(k) == segundos[idxSeg[j]]) { iA = k; break; }
            for (int k = 0; k < gb->getCantEquipos(); k++)
                if (gb->getEquipo(k) == segundos[idxSeg[j+1]]) { iB = k; break; }
            if (iA < 0 || iB < 0) continue;
            if (mejorTercero(ga, iA, gb, iB)) {
                // Intercambio manual (reemplaza swap() de <algorithm>)
                int tmp = idxSeg[j]; idxSeg[j] = idxSeg[j+1]; idxSeg[j+1] = tmp;
            }
        }
    }
    int peoresS[4]; int grupoPeoresS[4]; int nPS = 0;
    for (int i = 0; i < 4 && i < nS; i++) {
        iteraciones++;
        peoresS[nPS] = idxSeg[i];
        grupoPeoresS[nPS] = grupoSegundo[idxSeg[i]];
        nPS++;
    }
    bool peoresSUsado[4] = {false};

    // Emparejar primeros sin rival con los 4 peores segundos (de distinto grupo)
    for (int s = 0; s < nPS; s++) {
        iteraciones++;
        Equipo* seg = segundos[peoresS[s]];
        int gS = grupoPeoresS[s];
        Equipo* rival = nullptr;
        int rivalSR = -1;
        for (int r = 0; r < nSR && !rival; r++) {
            iteraciones++;
            int p = sinRival[r];
            if (primeroUsado[p]) continue;
            if (grupoPrimero[p] == gS) continue; // No pueden ser del mismo grupo
            rival = primeros[p];
            rivalSR = p;
        }
        if (rival && seg) {
            agregarPartido(rival, seg);
            primeroUsado[rivalSR] = true;
            segundoUsado[peoresS[s]] = true;
            peoresSUsado[s] = true;
        }
    }

    // PASO 3: Los mejores segundos restantes se enfrentan entre sí (de distinto grupo)
    Equipo* mejoresS[8]; int grupoMejoresS[8]; int nMS = 0;
    for (int i = 4; i < nS && nMS < 8; i++) {
        iteraciones++;
        int s = idxSeg[i];
        if (!segundoUsado[s]) {
            mejoresS[nMS] = segundos[s];
            grupoMejoresS[nMS] = grupoSegundo[s];
            nMS++;
        }
    }
    for (int s = 0; s < nPS; s++) {
        iteraciones++;
        if (!peoresSUsado[s] && !segundoUsado[peoresS[s]] && nMS < 8) {
            mejoresS[nMS] = segundos[peoresS[s]];
            grupoMejoresS[nMS] = grupoPeoresS[s];
            nMS++;
        }
    }

    bool mejoresSUsado[8] = {false};
    for (int i = 0; i < nMS; i++) {
        iteraciones++;
        if (mejoresSUsado[i]) continue;
        for (int j = i + 1; j < nMS; j++) {
            iteraciones++;
            if (mejoresSUsado[j]) continue;
            if (grupoMejoresS[i] == grupoMejoresS[j]) continue; // No del mismo grupo
            agregarPartido(mejoresS[i], mejoresS[j]);
            mejoresSUsado[i] = true;
            mejoresSUsado[j] = true;
            break;
        }
    }
}

// ============================================================
// Req IV: generarEstadisticas
// Despliega el informe final del torneo con:
//   - Ranking de los 4 primeros puestos
//   - Máximo goleador del campeón
//   - Top 3 goleadores del torneo
//   - Equipo con más goles históricos (tras actualización post-copa)
//   - Confederación dominante en R16, R8 y QF
// ============================================================
void Mundial::generarEstadisticas() {
    iteraciones = 0;
    cout << "\n=== Generando estadisticas finales... ===\n";
    cout << "\n========== ESTADISTICAS FINALES ==========\n";

    // Obtener campeón desde la fase Final
    Equipo* campeon    = getCampeon();
    Equipo* subcampeon = nullptr;
    Equipo* tercero    = nullptr;
    Equipo* cuarto     = nullptr;

    // Recorrer las fases para identificar subcampeón, tercero y cuarto
    for (int i = 0; i < cantFases; i++) {
        iteraciones++;
        if (fases[i] && fases[i]->getNombre() == "Final") {
            Partido* pf = fases[i]->getPartido(0);
            if (pf) {
                Equipo* e1 = pf->getEquipo1();
                Equipo* e2 = pf->getEquipo2();
                // El subcampeón es el que no ganó la final
                subcampeon = (campeon == e1) ? e2 : e1;
            }
        }
        if (fases[i] && fases[i]->getNombre() == "Tercer Puesto") {
            tercero = fases[i]->getGanadorPartido(0);
            Partido* p3 = fases[i]->getPartido(0);
            if (p3) {
                Equipo* e1 = p3->getEquipo1();
                Equipo* e2 = p3->getEquipo2();
                // El cuarto puesto es el perdedor del partido de tercer puesto
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
            // Buscar el jugador con más goles en la plantilla del campeón
            if (jug && jug->getGoles() > maxG) { maxG = jug->getGoles(); maxJug = jug; }
        }
        if (maxJug)
            cout << maxJug->getNombre() << " " << maxJug->getApellido()
                 << " (#" << maxJug->getNumeroCamiseta() << ") - " << maxG << " goles\n";
    }

    cout << "\n--- Top 3 Goleadores del Torneo ---\n";
    // Arreglos paralelos para los 3 mejores goleadores: jugador, goles, equipo
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
            // Inserción ordenada en el top 3 (desplaza hacia abajo si supera algún valor)
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

    // Contar presencia de confederaciones en R16, R8 y QF
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
                    // Incrementar contador de la confederación correspondiente
                    for (int c = 0; c < 6; c++) {
                        iteraciones++;
                        if (eq[q]->getConfederacion() == confNombres[c]) { confCount[c]++; break; }
                    }
                }
            }
        }
        // Buscar la confederación con mayor presencia en esta etapa
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

// Retorna el equipo ganador de la fase Final (el campeón del mundial)
Equipo* Mundial::getCampeon() {
    for (int i = 0; i < cantFases; i++)
        if (fases[i] && fases[i]->getNombre() == "Final")
            return fases[i]->getGanadorPartido(0);
    return nullptr;
}

// Recorre todos los equipos y retorna el que tiene más goles a favor históricos
// (estadísticas ya actualizadas con los partidos de la copa)
Equipo* Mundial::getEquipoMasGoles() {
    Equipo* mejor = nullptr; int max = -1;
    for (int i = 0; i < cantEquipos; i++) {
        iteraciones++;
        if (equipos[i] && equipos[i]->getGFA() > max) { max = equipos[i]->getGFA(); mejor = equipos[i]; }
    }
    return mejor;
}
