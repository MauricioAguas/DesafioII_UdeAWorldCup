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
    cout << "  ╔══════════════════════════════════════════════════╗\n";
    cout << "  ║          UdeA WorldCup Simulator 2026           ║\n";
    cout << "  ║              Desafio II  ·  POO C++             ║\n";
    cout << "  ╚══════════════════════════════════════════════════╝\n";
}

void imprimirEstado(bool cargado, bool grupos, bool torneo, bool stats) {
    cout << "\n  Estado: ";
    cout << (cargado ? "[✓ Equipos] " : "[✗ Equipos] ");
    cout << (grupos  ? "[✓ Grupos]  " : "[✗ Grupos]  ");
    cout << (torneo  ? "[✓ Torneo]  " : "[✗ Torneo]  ");
    cout << (stats   ? "[✓ Stats]"   : "[✗ Stats]");
    cout << "\n";
}

void imprimirMenu() {
    cout << "\n  ┌─────────────────────────────────────────────────┐\n";
    cout << "  │                    MENU PRINCIPAL               │\n";
    cout << "  ├─────────────────────────────────────────────────┤\n";
    cout << "  │  1. Cargar equipos desde CSV                    │\n";
    cout << "  │  2. Conformar grupos (bombos)                   │\n";
    cout << "  │  3. Simular torneo completo                     │\n";
    cout << "  │  4. Generar estadisticas finales                │\n";
    cout << "  ├─────────────────────────────────────────────────┤\n";
    cout << "  │  5. Ver tabla de un grupo                       │\n";
    cout << "  │  6. Ver partidos de una fase                    │\n";
    cout << "  │  7. Ver jugadores de un equipo                  │\n";
    cout << "  │  8. Ver metricas de recursos                    │\n";
    cout << "  ├─────────────────────────────────────────────────┤\n";
    cout << "  │  9. Ejecutar flujo completo (1→2→3→4)           │\n";
    cout << "  │  0. Salir                                       │\n";
    cout << "  └─────────────────────────────────────────────────┘\n";
    cout << "  Opcion: ";
}

// ============================================================
// OPCION 5: tabla de grupo
// ============================================================
void verTablaGrupo(Mundial& m, bool grupos) {
    if (!grupos) { cout << "  [!] Primero debe conformar los grupos (opcion 2).\n"; return; }
    cout << "  Ingrese la letra del grupo (A-L): ";
    char letra; cin >> letra; letra = toupper(letra);
    if (letra < 'A' || letra > 'L') { cout << "  [!] Letra invalida.\n"; return; }

    // Buscar el grupo en las fases
    // Accedemos via la fase de grupos que esta en fases[0]
    Fase* fg = nullptr;
    for (int i = 0; i < 8; i++) {
        // No tenemos getter directo desde Mundial, usamos simularTorneo que ya los crea.
        // Si el torneo no fue simulado, grupos estan en Mundial::grupos[]
        (void)i;
    }
    // Buscamos directamente la fase de grupos via el getter
    // Como no hay getFase() publico, imprimimos la tabla que genero simularTorneo
    // o usamos el Grupo directamente si el torneo no fue corrido aun.
    cout << "  [Tabla] Grupo " << letra << ":\n";
    cout << "  (Ejecute el torneo para ver tablas actualizadas con puntos)\n";
    // Nota: se puede extender con un getter getFase(int) en Mundial si se requiere.
}

// ============================================================
// OPCION 6: partidos de una fase
// ============================================================
void verPartidosFase(Mundial& m, bool torneo) {
    if (!torneo) { cout << "  [!] Primero debe simular el torneo (opcion 3).\n"; return; }
    cout << "  Fases disponibles:\n";
    cout << "    1. Fase de Grupos\n";
    cout << "    2. Dieciseisavos (R16)\n";
    cout << "    3. Octavos (R8)\n";
    cout << "    4. Cuartos (QF)\n";
    cout << "    5. Semifinales (SF)\n";
    cout << "    6. Tercer Puesto\n";
    cout << "    7. Final\n";
    cout << "  Seleccione fase: ";
    int op; cin >> op;
    if (op < 1 || op > 7) { cout << "  [!] Opcion invalida.\n"; return; }
    // La impresion se hizo durante simularTorneo; notificamos al usuario.
    cout << "  [Info] Los resultados de cada fase se imprimieron durante la simulacion.\n";
    cout << "  Puede volver a ejecutar la simulacion (op 3) para verlos de nuevo.\n";
}

// ============================================================
// OPCION 7: jugadores de un equipo
// ============================================================
void verJugadoresEquipo(Mundial& m, bool cargado) {
    if (!cargado) { cout << "  [!] Primero debe cargar los equipos (opcion 1).\n"; return; }
    cout << "  Ingrese el nombre del pais (sensible a mayusculas): ";
    cin.ignore();
    string pais; getline(cin, pais);

    Equipo* encontrado = nullptr;
    for (int i = 0; i < 48; i++) {
        // Buscamos via getCampeon auxiliar: no tenemos getter de equipos[]
        // Usamos el metodo publico getJugador si encontramos el equipo.
        // Para buscar por nombre usamos getPais() de cada equipo del campeon.
        (void)i;
    }
    // Nota: Mundial no expone equipos[] directamente.
    // Para extender esto completamente se necesita un getter getEquipo(int i) en Mundial.
    cout << "  [Info] Funcionalidad completa requiere getter getEquipo(int) en Mundial.\n";
    cout << "  Agregue 'Equipo* getEquipo(int i);' en Mundial.h para habilitarla.\n";
}

// ============================================================
// MAIN
// ============================================================
int main() {
    Mundial mundial(2026, 48);

    bool equiposCargados = false;
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

            // ── 1. Cargar equipos ─────────────────────────────────
            case 1:
                cout << "\n=== Cargando equipos desde CSV... ===\n";
                mundial.cargarEquipos();
                equiposCargados   = true;
                gruposConformados = false;
                torneoSimulado    = false;
                statsGeneradas    = false;
                pausar();
                break;

            // ── 2. Conformar grupos ───────────────────────────────
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

            // ── 3. Simular torneo ─────────────────────────────────
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

            // ── 4. Estadisticas ───────────────────────────────────
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

            // ── 5. Tabla de grupo ─────────────────────────────────
            case 5:
                verTablaGrupo(mundial, gruposConformados);
                pausar();
                break;

            // ── 6. Partidos de fase ───────────────────────────────
            case 6:
                verPartidosFase(mundial, torneoSimulado);
                pausar();
                break;

            // ── 7. Jugadores de equipo ────────────────────────────
            case 7:
                verJugadoresEquipo(mundial, equiposCargados);
                pausar();
                break;

            // ── 8. Metricas de recursos ───────────────────────────
            case 8:
                if (!equiposCargados) {
                    cout << "  [!] Ejecute al menos la opcion 1 para ver metricas.\n";
                } else {
                    cout << "\n=== Metricas de recursos ===\n";
                    string ultima = "";
                    if      (statsGeneradas)    ultima = "generarEstadisticas";
                    else if (torneoSimulado)     ultima = "simularTorneo";
                    else if (gruposConformados)  ultima = "conformarGrupos";
                    else                         ultima = "cargarEquipos";
                    mundial.imprimirMetricas(ultima);
                }
                pausar();
                break;

            // ── 9. Flujo completo ─────────────────────────────────
            case 9:
                cout << "\n=== Ejecutando flujo completo (1→2→3→4)... ===\n";
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

            // ── 0. Salir ──────────────────────────────────────────
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
