#include "Mundial.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

Mundial::Mundial() {
    anio        = 0;
    cantEquipos = 0;
    cantFases   = 0;
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
    for (int i = 0; i < cantEquipos; i++)
        if (equipos[i] != nullptr) delete equipos[i];
    for (int i = 0; i < 12; i++)
        if (grupos[i] != nullptr) delete grupos[i];
    for (int i = 0; i < cantFases; i++)
        if (fases[i] != nullptr) delete fases[i];
}

void Mundial::cargarEquipos() {
    // TODO: leer desde seleccionesclasificadasmundial.csv
    // Por ahora se crean equipos de prueba
    cout << "[Mundial] cargarEquipos() pendiente de implementacion con CSV\n";
}

void Mundial::conformarGrupos() {
    // Crear 12 grupos A-L
    char letras[12] = {'A','B','C','D','E','F','G','H','I','J','K','L'};
    for (int g = 0; g < 12; g++)
        grupos[g] = new Grupo(letras[g]);

    // TODO: implementar sorteo con bombos y restricciones de confederacion
    // Por ahora asignar 4 equipos consecutivos a cada grupo
    for (int g = 0; g < 12; g++)
        for (int e = 0; e < 4; e++)
            if (equipos[g * 4 + e] != nullptr)
                grupos[g]->agregarEquipo(equipos[g * 4 + e]);

    cout << "[Mundial] Grupos conformados (sorteo completo pendiente)\n";
    for (int g = 0; g < 12; g++) {
        cout << "Grupo " << letras[g] << ": ";
        for (int e = 0; e < grupos[g]->getCantEquipos(); e++)
            cout << grupos[g]->getEquipo(e)->getPais() << "  ";
        cout << "\n";
    }
}

void Mundial::simularTorneo() {
    // --- Fase de grupos ---
    Fase* faseGrupos = new Fase("Fase de Grupos", "grupos", "2026-06-20");
    for (int g = 0; g < 12; g++) faseGrupos->agregarGrupo(grupos[g]);
    faseGrupos->configurarPartidos();
    faseGrupos->simularPartidos();
    faseGrupos->generarTablas();
    faseGrupos->imprimirResultados();
    fases[cantFases++] = faseGrupos;

    // --- R16 (32 equipos: 12 primeros + 12 segundos + 8 mejores terceros) ---
    Fase* faseR16 = new Fase("Dieciseisavos (R16)", "eliminacion", "2026-07-10");
    armarR16(faseGrupos, faseR16);
    faseR16->simularPartidos();
    faseR16->imprimirResultados();
    fases[cantFases++] = faseR16;

    // --- R8 (16 ganadores de R16) ---
    Fase* faseR8 = new Fase("Octavos (R8)", "eliminacion", "2026-07-10");
    for (int i = 0; i < 16; i++) {
        Equipo* g = faseR16->getGanadorPartido(i);
        Equipo* h = faseR16->getGanadorPartido(i % 2 == 0 ? i+1 : i-1);
        if (g && h && i % 2 == 0)
            faseR8->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede", g, h));
    }
    faseR8->simularPartidos();
    faseR8->imprimirResultados();
    fases[cantFases++] = faseR8;

    // --- QF ---
    Fase* faseQF = new Fase("Cuartos (QF)", "eliminacion", "2026-07-10");
    for (int i = 0; i < 8; i++) {
        Equipo* g = faseR8->getGanadorPartido(i);
        Equipo* h = faseR8->getGanadorPartido(i % 2 == 0 ? i+1 : i-1);
        if (g && h && i % 2 == 0)
            faseQF->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede", g, h));
    }
    faseQF->simularPartidos();
    faseQF->imprimirResultados();
    fases[cantFases++] = faseQF;

    // --- SF ---
    Fase* faseSF = new Fase("Semifinales (SF)", "eliminacion", "2026-07-10");
    for (int i = 0; i < 4; i++) {
        Equipo* g = faseQF->getGanadorPartido(i);
        Equipo* h = faseQF->getGanadorPartido(i % 2 == 0 ? i+1 : i-1);
        if (g && h && i % 2 == 0)
            faseSF->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede", g, h));
    }
    faseSF->simularPartidos();
    faseSF->imprimirResultados();
    fases[cantFases++] = faseSF;

    // --- Tercer puesto ---
    // Los perdedores de SF son los equipos que no ganaron
    Equipo* sf1g = faseSF->getGanadorPartido(0);
    Equipo* sf2g = faseSF->getGanadorPartido(1);
    Equipo* sf1e1 = faseSF->getPartido(0)->getEquipo1();
    Equipo* sf1e2 = faseSF->getPartido(0)->getEquipo2();
    Equipo* sf2e1 = faseSF->getPartido(1)->getEquipo1();
    Equipo* sf2e2 = faseSF->getPartido(1)->getEquipo2();
    Equipo* perdedor1 = (sf1g == sf1e1) ? sf1e2 : sf1e1;
    Equipo* perdedor2 = (sf2g == sf2e1) ? sf2e2 : sf2e1;

    Fase* fase3er = new Fase("Tercer Puesto", "eliminacion", "2026-07-10");
    fase3er->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede", perdedor1, perdedor2));
    fase3er->simularPartidos();
    fase3er->imprimirResultados();
    fases[cantFases++] = fase3er;

    // --- Final ---
    Fase* faseFinal = new Fase("Final", "eliminacion", "2026-07-10");
    faseFinal->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede", sf1g, sf2g));
    faseFinal->simularPartidos();
    faseFinal->imprimirResultados();
    fases[cantFases++] = faseFinal;
}

void Mundial::armarR16(Fase* faseGrupos, Fase* faseR16) {
    // TODO: implementar logica completa del enunciado:
    // 12 primeros, 12 segundos, 8 mejores terceros
    // Restriccion: no pueden coincidir equipos del mismo grupo
    // Por ahora: emparejar primero vs segundo de grupos consecutivos
    for (int g = 0; g < 12; g += 2) {
        Grupo* ga = faseGrupos->getGrupo(g);
        Grupo* gb = faseGrupos->getGrupo(g + 1);
        if (ga && gb) {
            faseR16->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede",
                ga->obtenerClasificados()[0], gb->obtenerClasificados()[1]));
            faseR16->agregarPartido(new Partido("2026-07-10", "00:00", "nombreSede",
                gb->obtenerClasificados()[0], ga->obtenerClasificados()[1]));
        }
    }
}

void Mundial::generarEstadisticas() {
    cout << "\n========== ESTADISTICAS FINALES ==========\n";
    // TODO: implementar estadisticas completas del enunciado
    cout << "[Mundial] generarEstadisticas() pendiente de implementacion\n";
}

Equipo* Mundial::getCampeon() {
    // El campeon es el ganador de la Final
    for (int i = 0; i < cantFases; i++) {
        if (fases[i] != nullptr && fases[i]->getNombre() == "Final")
            return fases[i]->getGanadorPartido(0);
    }
    return nullptr;
}

Equipo* Mundial::getEquipoMasGoles() {
    Equipo* mejor = nullptr;
    int maxGoles = -1;
    for (int i = 0; i < cantEquipos; i++) {
        if (equipos[i] != nullptr && equipos[i]->getGFA() > maxGoles) {
            maxGoles = equipos[i]->getGFA();
            mejor = equipos[i];
        }
    }
    return mejor;
}
