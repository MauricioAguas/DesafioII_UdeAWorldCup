TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
TARGET = DesafioII_UdeAWorldCup

HEADERS += \
    hds/Equipo.h \
    hds/EstadisticasEquipo.h \
    hds/EstadisticasJugador.h \
    hds/Fase.h \
    hds/Grupo.h \
    hds/Jugador.h \
    hds/Mundial.h \
    hds/Partido.h \
    hds/Resultado.h

SOURCES += \
    src/main.cpp \
    src/Equipo.cpp \
    src/EstadisticasEquipo.cpp \
    src/EstadisticasJugador.cpp \
    src/Fase.cpp \
    src/Grupo.cpp \
    src/Jugador.cpp \
    src/Mundial.cpp \
    src/Partido.cpp \
    src/Resultado.cpp
