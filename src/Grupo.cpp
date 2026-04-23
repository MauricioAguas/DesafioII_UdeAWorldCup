#include "Grupo.h"
#include <iostream>
using namespace std;

Grupo::Grupo() {
    letra       = ' ';
    cantEquipos = 0;
    for (int i = 0; i < 4; i++) {
        equipos[i]   = nullptr;
        puntos[i]    = 0;
        difGoles[i]  = 0;
        golesFavor[i]= 0;
    }
    for (int i = 0; i < 6; i++) partidos[i] = nullptr;
}

Grupo::Grupo(char letra) : Grupo() {
    this->letra = letra;
}

Grupo::~Grupo() {
    for (int i = 0; i < 6; i++)
        if (partidos[i] != nullptr) delete partidos[i];
    // Los Equipo* son propiedad del Mundial, no se eliminan aqui
}

void Grupo::agregarEquipo(Equipo* e) {
    if (cantEquipos < 4)
        equipos[cantEquipos++] = e;
}

// Genera los 6 partidos round-robin asignando fechas dentro del lapso de 19 dias.
// Restricciones del enunciado:
//   - max 4 partidos por dia
//   - ningun equipo juega mas de 1 partido en lapso de 3 dias
void Grupo::configurarPartidos(string fechaBase) {
    // Pares round-robin para 4 equipos: (0,1),(0,2),(0,3),(1,2),(1,3),(2,3)
    int e1idx[6] = {0, 0, 0, 1, 1, 2};
    int e2idx[6] = {1, 2, 3, 2, 3, 3};

    // Dias asignados para cada partido del grupo (dentro de 19 dias, separados >= 3)
    // Cada equipo juega en dias: partido 0 dia 0, partido 1 dia 4, partido 2 dia 8
    // Los 6 partidos se distribuyen en 3 fechas distintas por grupo
    // Se usa un offset por letra de grupo para no saturar el mismo dia entre grupos
    int offset = (letra - 'A') % 4; // 0-3 para escalonar
    int diasBase[3] = {0 + offset, 4 + offset, 8 + offset};

    // Extraer anio, mes, dia de fechaBase (formato "YYYY-MM-DD")
    int anio = stoi(fechaBase.substr(0, 4));
    int mes  = stoi(fechaBase.substr(5, 2));
    int dia  = stoi(fechaBase.substr(8, 2));

    // Dias en cada mes (simplificado, no bisiesto)
    int diasMes[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

    for (int p = 0; p < 6; p++) {
        // Determinar a cual de las 3 fechas pertenece este partido
        // Partidos 0,1 -> fecha 0 | partidos 2,3 -> fecha 1 | partidos 4,5 -> fecha 2
        int grupoFecha = p / 2;
        int diaPartido = dia + diasBase[grupoFecha];
        int mesPartido = mes;
        int anioPartido = anio;

        // Ajustar desbordamiento de mes
        while (diaPartido > diasMes[mesPartido]) {
            diaPartido -= diasMes[mesPartido];
            mesPartido++;
            if (mesPartido > 12) { mesPartido = 1; anioPartido++; }
        }

        // Formatear fecha "YYYY-MM-DD"
        string f = to_string(anioPartido) + "-";
        if (mesPartido < 10) f += "0";
        f += to_string(mesPartido) + "-";
        if (diaPartido < 10) f += "0";
        f += to_string(diaPartido);

        partidos[p] = new Partido(f, "00:00", "nombreSede",
                                  equipos[e1idx[p]], equipos[e2idx[p]]);
    }
}

void Grupo::simularPartidos() {
    for (int i = 0; i < 6; i++)
        if (partidos[i] != nullptr) partidos[i]->simular();
}

// Calcula puntos, diferencia de goles y goles a favor; luego ordena (bubble sort)
void Grupo::generarTablaClasif() {
    // Reiniciar contadores
    for (int i = 0; i < 4; i++) {
        puntos[i]    = 0;
        difGoles[i]  = 0;
        golesFavor[i]= 0;
    }

    // Mapeo equipo -> indice en arreglo equipos[]
    for (int p = 0; p < 6; p++) {
        if (partidos[p] == nullptr) continue;
        Resultado* res = partidos[p]->getResultado();
        if (res == nullptr) continue;

        int gf1 = res->getGfEquipo1();
        int gf2 = res->getGfEquipo2();

        // Encontrar indices de los equipos de este partido
        Equipo* eq1 = partidos[p]->getEquipo1();
        Equipo* eq2 = partidos[p]->getEquipo2();
        int idx1 = -1, idx2 = -1;
        for (int i = 0; i < 4; i++) {
            if (equipos[i] == eq1) idx1 = i;
            if (equipos[i] == eq2) idx2 = i;
        }
        if (idx1 == -1 || idx2 == -1) continue;

        golesFavor[idx1] += gf1;
        golesFavor[idx2] += gf2;
        difGoles[idx1]   += gf1 - gf2;
        difGoles[idx2]   += gf2 - gf1;

        if (gf1 > gf2)      { puntos[idx1] += 3; }
        else if (gf1 < gf2) { puntos[idx2] += 3; }
        else                { puntos[idx1]++; puntos[idx2]++; }
    }

    // Ordenar por puntos desc, luego difGoles desc, luego golesFavor desc (bubble sort)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i; j++) {
            bool swap = false;
            if (puntos[j] < puntos[j+1]) swap = true;
            else if (puntos[j] == puntos[j+1] && difGoles[j] < difGoles[j+1]) swap = true;
            else if (puntos[j] == puntos[j+1] && difGoles[j] == difGoles[j+1] && golesFavor[j] < golesFavor[j+1]) swap = true;

            if (swap) {
                Equipo* tmpE   = equipos[j]; equipos[j]   = equipos[j+1]; equipos[j+1]   = tmpE;
                int     tmpP   = puntos[j];  puntos[j]    = puntos[j+1];  puntos[j+1]    = tmpP;
                int     tmpD   = difGoles[j]; difGoles[j] = difGoles[j+1]; difGoles[j+1] = tmpD;
                int     tmpG   = golesFavor[j]; golesFavor[j] = golesFavor[j+1]; golesFavor[j+1] = tmpG;
            }
        }
    }
}

void Grupo::imprimirTabla() {
    cout << "\n=== Grupo " << letra << " ===";
    cout << "\nPos | Equipo                | Pts | DG  | GF";
    cout << "\n----|----------------------|-----|-----|----";
    for (int i = 0; i < cantEquipos; i++) {
        cout << "\n " << (i+1) << "  | "
             << equipos[i]->getPais();
        // Padding
        int len = equipos[i]->getPais().length();
        for (int s = len; s < 21; s++) cout << " ";
        cout << "| " << puntos[i]
             << "   | " << difGoles[i]
             << "   | " << golesFavor[i];
    }
    cout << endl;
}

// Retorna los 4 equipos ya ordenados (debe llamarse despues de generarTablaClasif)
Equipo** Grupo::obtenerClasificados() {
    return equipos;
}

char  Grupo::getLetra()       { return letra; }
Equipo* Grupo::getEquipo(int i) { return (i >= 0 && i < cantEquipos) ? equipos[i] : nullptr; }
int   Grupo::getCantEquipos() { return cantEquipos; }
int   Grupo::getPuntos(int i) { return (i >= 0 && i < 4) ? puntos[i] : 0; }
