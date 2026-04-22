#include "Equipo.h"
#include <iostream>
using namespace std;

Equipo::Equipo() {
    pais = "";
    confederacion = "";
    ranking = 0;
    DT = "";
    plantilla = new Jugador*[26];
    for (int i = 0; i < 26; i++) plantilla[i] = nullptr;
    estadisticas = new EstadisticasEquipo();
}

Equipo::Equipo(string pais, string confederacion, int ranking, string DT) {
    this->pais = pais;
    this->confederacion = confederacion;
    this->ranking = ranking;
    this->DT = DT;
    plantilla = new Jugador*[26];
    for (int i = 0; i < 26; i++) plantilla[i] = nullptr;
    estadisticas = new EstadisticasEquipo();
}

Equipo::~Equipo() {
    for (int i = 0; i < 26; i++) {
        if (plantilla[i] != nullptr)
            delete plantilla[i];
    }
    delete[] plantilla;
    delete estadisticas;
}

void Equipo::cargarJugadores() {
    // Se implementara con lectura de archivo CSV
    // Por ahora se generan jugadores con datos basicos
    for (int i = 0; i < 26; i++) {
        plantilla[i] = new Jugador("Jugador", pais, i + 1);
    }
}

Jugador** Equipo::seleccionarConvocados() {
    // Retorna los primeros 11 jugadores de la plantilla
    Jugador** convocados = new Jugador*[11];
    for (int i = 0; i < 11; i++) {
        convocados[i] = plantilla[i];
    }
    return convocados;
}

void Equipo::actualizarEstadisticas(int gf, int gc, int resultado) {
    estadisticas->actualizar(gf, gc, resultado);
}

int Equipo::getGFA() {
    return estadisticas->getGolesFavor();
}

int Equipo::getGEC() {
    return estadisticas->getGolesContra();
}

string Equipo::getPais() {
    return pais;
}

int Equipo::getRanking() {
    return ranking;
}

string Equipo::getConfederacion() {
    return confederacion;
}

ostream& operator<<(ostream& os, const Equipo& e) {
    os << "Equipo: "        << e.pais
       << " | Conf: "      << e.confederacion
       << " | Ranking: "   << e.ranking
       << " | DT: "        << e.DT;
    return os;
}
