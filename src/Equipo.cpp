#include "../include/Equipo.h"
#include <iostream>
using namespace std;

Equipo::Equipo() {
    pais = confederacion = DT = "";
    ranking = 0;
    plantilla = new Jugador*[26];
    for (int i = 0; i < 26; i++) plantilla[i] = nullptr;
    estadisticas = new EstadisticasEquipo();
}

Equipo::Equipo(string pais, string confederacion, int ranking, string DT) {
    this->pais          = pais;
    this->confederacion = confederacion;
    this->ranking       = ranking;
    this->DT            = DT;
    plantilla = new Jugador*[26];
    for (int i = 0; i < 26; i++) plantilla[i] = nullptr;
    estadisticas = new EstadisticasEquipo();
}

Equipo::~Equipo() {
    for (int i = 0; i < 26; i++)
        if (plantilla[i] != nullptr) delete plantilla[i];
    delete[] plantilla;
    delete estadisticas;
}

void Equipo::cargarJugadores() {
    // Repartir goles historicos del equipo uniformemente entre los 26 jugadores
    int totalGoles   = estadisticas->getGolesFavor();
    int golesPorJug  = totalGoles / 26;
    int golesResto   = totalGoles % 26;   // primeros 'golesResto' jugadores reciben 1 extra

    for (int i = 0; i < 26; i++) {
        int goles = golesPorJug + (i < golesResto ? 1 : 0);
        plantilla[i] = new Jugador(
            "nombre"   + to_string(i + 1),
            "apellido" + to_string(i + 1),
            i + 1,   // numero de camiseta
            goles    // goles historicos iniciales
        );
    }
}

Jugador** Equipo::seleccionarConvocados() {
    // Seleccion aleatoria de 11 jugadores distintos de la plantilla
    Jugador** convocados = new Jugador*[11];
    bool seleccionado[26] = {false};
    int cnt = 0;
    while (cnt < 11) {
        int idx = rand() % 26;
        if (!seleccionado[idx]) {
            seleccionado[idx] = true;
            convocados[cnt++]  = plantilla[idx];
        }
    }
    return convocados;
}

void Equipo::actualizarEstadisticas(int gf, int gc, int resultado) {
    estadisticas->actualizar(gf, gc, resultado);
}

void Equipo::setEstadisticasIniciales(int gf, int gc, int gan, int emp, int per) {
    estadisticas->setGolesFavor(gf);
    estadisticas->setGolesContra(gc);
    estadisticas->setGanados(gan);
    estadisticas->setEmpatados(emp);
    estadisticas->setPerdidos(per);
}

int    Equipo::getGFA()           { return estadisticas->getGolesFavor(); }
int    Equipo::getGEC()           { return estadisticas->getGolesContra(); }
string Equipo::getPais()          { return pais; }
int    Equipo::getRanking()       { return ranking; }
string Equipo::getConfederacion() { return confederacion; }

ostream& operator<<(ostream& os, const Equipo& e) {
    os << "Equipo: " << e.pais
       << " | Conf: "    << e.confederacion
       << " | Ranking: " << e.ranking
       << " | DT: "      << e.DT;
    return os;
}
