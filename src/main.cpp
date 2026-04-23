#include <iostream>
#include "../include/Mundial.h"
using namespace std;

int main() {
    Mundial mundial(2026, 48);
    mundial.cargarEquipos();
    mundial.conformarGrupos();
    mundial.simularTorneo();
    mundial.generarEstadisticas();
    return 0;
}
