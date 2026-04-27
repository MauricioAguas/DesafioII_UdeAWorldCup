#include <iostream>
#include <string>
#include "../hds/Mundial.h"
using namespace std;

// ============================================================
// HELPERS DE PRESENTACION
// ============================================================
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

void imprimirBanner() {
    cout << "\n";
    cout << "  +--------------------------------------------------+\n";
    cout << "  |          UdeA WorldCup Simulator 2026           |\n";
    cout << "  |              Desafio II  -  POO C++             |\n";
    cout << "  +--------------------------------------------------+\n";
}

void imprimirEstado(bool cargado, bool grupos, bool torneo, bool stats) {
    cout << "\n  Estado: ";
    cout << (cargado ? "[OK Equipos] " : "[-- Equipos] ");
    cout << (grupos  ? "[OK Grupos]  " : "[-- Grupos]  ");
    cout << (torneo  ? "[OK Torneo]  " : "[-- Torneo]  ");
    cout << (stats   ? "[OK Stats]"    : "[-- Stats]");
    cout << "\n";
}

void imprimirMenu() {
    cout << "\n  +-------------------------------------------------+\n";
    cout << "  |                 MENU PRINCIPAL                  |\n";
    cout << "  +-------------------------------------------------+\n";
    cout << "  |  1. Cargar equipos desde CSV                    |\n";
    cout << "  |  2. Conformar grupos (bombos)                   |\n";
    cout << "  |  3. Simular torneo completo                     |\n";
    cout << "  |  4. Generar estadisticas finales                |\n";
    cout << "  +-------------------------------------------------+\n";
    cout << "  |  5. Ver tabla de un grupo                       |\n";
    cout << "  |  6. Ver partidos de una fase                    |\n";
    cout << "  |  7. Ver jugadores de un equipo                  |\n";
    cout << "  |  8. Ver metricas de recursos                    |\n";
    cout << "  +-------------------------------------------------+\n";
    cout << "  |  9. Ejecutar flujo completo (1->2->3->4)        |\n";
    cout << "  |  0. Salir                                       |\n";
    cout << "  +-------------------------------------------------+\n";
    cout << "  Opcion: ";
}
// ============================================================
// OPCION 5: tabla de un grupo especifico
// ============================================================
void verTablaGrupo(Mundial& m, bool grupos) {
    if (!grupos) { cout << "  [!] Primero debe conformar los grupos (opcion 2).\n"; return; }
    cout << "  Ingrese la letra del grupo (A-L): ";
    char letra; cin >> letra; letra = toupper(letra);
    if (letra < 'A' || letra > 'L') { cout << "  [!] Letra invalida.\n"; return; }

    int idx = letra - 'A';
    Grupo* g = m.getGrupo(idx);
    if (!g) { cout << "  [!] Grupo no encontrado.\n"; return; }

    cout << "\n  === Grupo " << letra << " ==="  << "\n";
    cout << "  " << left;
    cout.width(22); cout << "Equipo";
    cout.width(6);  cout << "PJ";
    cout.width(6);  cout << "G";
    cout.width(6);  cout << "E";
    cout.width(6);  cout << "P";
    cout.width(6);  cout << "GF";
    cout.width(6);  cout << "GC";
    cout.width(6);  cout << "DIF";
    cout.width(6);  cout << "PTS";
    cout << "\n";
    cout << "  " << string(68, '-') << "\n";

    for (int i = 0; i < g->getCantEquipos(); i++) {
        Equipo* e = g->getEquipo(i);
        if (!e) continue;
        EstadisticasEquipo* st = e->getEstadisticas();
        int pts = st->getGanados()*3 + st->getEmpatados();
        cout << "  ";
        cout.width(22); cout << left << e->getPais();
        cout.width(6);  cout << e->getPartidosJugados();
        cout.width(6);  cout << st->getGanados();
        cout.width(6);  cout << st->getEmpatados();
        cout.width(6);  cout << st->getPerdidos();
        cout.width(6);  cout << st->getGolesFavor();
        cout.width(6);  cout << st->getGolesContra();
        cout.width(6);  cout << (st->getGolesFavor() - st->getGolesContra());
        cout.width(6);  cout << pts;
        cout << "\n";
    }
}

// ============================================================
// OPCION 6: partidos de una fase
// ============================================================
void verPartidosFase(Mundial& m, bool torneo) {
    if (!torneo) { cout << "  [!] Primero debe simular el torneo (opcion 3).\n"; return; }

    const char* nombres[] = {
        "Fase de Grupos", "Dieciseisavos (R16)", "Octavos (R8)",
        "Cuartos (QF)",  "Semifinales (SF)",   "Tercer Puesto", "Final"
    };
    cout << "  Fases disponibles:\n";
    for (int i = 0; i < 7; i++)
        cout << "    " << (i+1) << ". " << nombres[i] << "\n";
    cout << "  Seleccione fase (1-7): ";
    int op; cin >> op;
    if (op < 1 || op > 7) { cout << "  [!] Opcion invalida.\n"; return; }

    // Buscar la fase por nombre
    Fase* fase = nullptr;
    for (int i = 0; i < m.getCantFases(); i++) {
        Fase* f = m.getFase(i);
        if (f && f->getNombre() == nombres[op-1]) { fase = f; break; }
    }
    if (!fase) { cout << "  [!] Fase no disponible.\n"; return; }

    cout << "\n  === " << fase->getNombre() << " ===\n";
    cout << "  " << string(50, '-') << "\n";
    for (int p = 0; p < fase->getCantPartidos(); p++) {
        Partido* par = fase->getPartido(p);
        if (!par) continue;
        Resultado* res = par->getResultado();
        cout << "  " << par->getEquipo1()->getPais();
        if (res)
            cout << "  " << res->getGfEquipo1() << " - " << res->getGfEquipo2();
        else
            cout << "  vs";
        cout << "  " << par->getEquipo2()->getPais();
        if (res && res->getHuboProrroga()) cout << "  (prorroga)";
        cout << "\n";
    }
}

// ============================================================
// OPCION 7: jugadores de un equipo
// ============================================================
void verJugadoresEquipo(Mundial& m, bool cargado) {
    if (!cargado) { cout << "  [!] Primero debe cargar los equipos (opcion 1).\n"; return; }
    cout << "  Ingrese el nombre del pais: ";
    cin.ignore();
    string pais; getline(cin, pais);

    Equipo* encontrado = nullptr;
    for (int i = 0; i < m.getCantEquipos(); i++) {
        Equipo* e = m.getEquipo(i);
        if (e && e->getPais() == pais) { encontrado = e; break; }
    }
    if (!encontrado) {
        cout << "  [!] Equipo '" << pais << "' no encontrado. Verifique mayusculas.\n";
        return;
    }

    cout << "\n  === Plantilla: " << encontrado->getPais()
         << " (" << encontrado->getConfederacion() << ") ==="
         << "  Ranking FIFA: " << encontrado->getRanking() << "\n";
    cout << "  " << string(52, '-') << "\n";
    cout << "  #  " << left;
    cout.width(22); cout << "Nombre";
    cout.width(14); cout << "Apellido";
    cout << "Goles" << "\n";
    cout << "  " << string(52, '-') << "\n";

    for (int j = 0; j < encontrado->getTamanoPlantilla(); j++) {
        Jugador* jug = encontrado->getJugador(j);
        if (!jug) continue;
        cout << "  ";
        cout.width(3);  cout << left << jug->getNumeroCamiseta();
        cout.width(22); cout << jug->getNombre();
        cout.width(14); cout << jug->getApellido();
        cout << jug->getGoles() << "\n";
    }
}

// ============================================================
// MAIN
// ============================================================
int main() {
    Mundial mundial(2026, 48);

    bool equiposCargados  = false;
    bool gruposConformados = false;
    bool torneoSimulado   = false;
    bool statsGeneradas   = false;

    int opcion = -1;

    while (opcion != 0) {
        limpiarPantalla();
        imprimirBanner();
        imprimirEstado(equiposCargados, gruposConformados, torneoSimulado, statsGeneradas);
        imprimirMenu();
        cin >> opcion;

        limpiarPantalla();

        switch (opcion) {

            case 1:
                cout << "\n=== Cargando equipos desde CSV... ===\n";
                mundial.cargarEquipos();
                equiposCargados   = true;
                gruposConformados = false;
                torneoSimulado    = false;
                statsGeneradas    = false;
                pausar();
                break;

            case 2:
                if (!equiposCargados) {
                    cout << "  [!] Cargue los equipos primero (opcion 1).\n";
                } else {
                    cout << "\n=== Conformando grupos por bombos... ===\n";
                    mundial.conformarGrupos();
                    gruposConformados = true;
                    torneoSimulado    = false;
                    statsGeneradas    = false;
                }
                pausar();
                break;

            case 3:
                if (!gruposConformados) {
                    cout << "  [!] Conforme los grupos primero (opcion 2).\n";
                } else {
                    cout << "\n=== Simulando torneo completo... ===\n";
                    mundial.simularTorneo();
                    torneoSimulado = true;
                    statsGeneradas = false;
                }
                pausar();
                break;

            case 4:
                if (!torneoSimulado) {
                    cout << "  [!] Simule el torneo primero (opcion 3).\n";
                } else {
                    cout << "\n=== Generando estadisticas finales... ===\n";
                    mundial.generarEstadisticas();
                    statsGeneradas = true;
                }
                pausar();
                break;

            case 5:
                verTablaGrupo(mundial, gruposConformados);
                pausar();
                break;

            case 6:
                verPartidosFase(mundial, torneoSimulado);
                pausar();
                break;

            case 7:
                verJugadoresEquipo(mundial, equiposCargados);
                pausar();
                break;

            case 8:
                if (!equiposCargados) {
                    cout << "  [!] Ejecute al menos la opcion 1 para ver metricas.\n";
                } else {
                    string ultima = "";
                    if      (statsGeneradas)   ultima = "generarEstadisticas";
                    else if (torneoSimulado)    ultima = "simularTorneo";
                    else if (gruposConformados) ultima = "conformarGrupos";
                    else                        ultima = "cargarEquipos";
                    mundial.imprimirMetricas(ultima);
                }
                pausar();
                break;

            case 9:
                cout << "\n=== Ejecutando flujo completo (1-2-3-4)... ===\n";
                mundial.cargarEquipos();
                equiposCargados = true;
                mundial.conformarGrupos();
                gruposConformados = true;
                mundial.simularTorneo();
                torneoSimulado = true;
                mundial.generarEstadisticas();
                statsGeneradas = true;
                cout << "\n[OK] Flujo completo finalizado.\n";
                pausar();
                break;

            case 0:
                cout << "\n  Hasta luego!\n\n";
                break;

            default:
                cout << "  [!] Opcion invalida. Intente de nuevo.\n";
                pausar();
                break;
        }
    }

    return 0;
}
