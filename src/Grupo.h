#ifndef GRUPO_H
#define GRUPO_H

#include "Equipo.h"

class Grupo {
private:
    char letra;
    int cantEquipos;

public:
    Grupo();
    Grupo(char letra);
    ~Grupo();

    void agregarEquipo(Equipo* e);
    void generarTablaClasif();
    Equipo** obtenerClasificados();
};

#endif
