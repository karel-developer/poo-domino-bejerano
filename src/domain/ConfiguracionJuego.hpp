#pragma once
#include <bits/stdc++.h>
using namespace std;

namespace domino {

enum ModoJuego {
    MODO_SOLO,
    MODO_CUATRO_JUGADORES
};

enum TipoJugador {
    HUMANO,
    IA
};

struct ConfiguracionJuego {
    ModoJuego modo;
    vector<TipoJugador> tipos;
    vector<string> nombres;
    int puntosParaGanar;
    int fichasPorJugador;

    ConfiguracionJuego()
        : modo(MODO_SOLO), puntosParaGanar(100), fichasPorJugador(10) {
        tipos = {HUMANO, IA, IA, IA};
        nombres = {"Jugador 1", "IA 1", "IA 2", "IA 3"};
    }

    static ConfiguracionJuego solo() {
        ConfiguracionJuego c;
        c.modo = MODO_SOLO;
        c.tipos = {HUMANO, IA, IA, IA};
        c.nombres = {"Tu", "Computadora 1", "Computadora 2", "Computadora 3"};
        return c;
    }

    static ConfiguracionJuego cuatroJugadores() {
        ConfiguracionJuego c;
        c.modo = MODO_CUATRO_JUGADORES;
        c.tipos = {HUMANO, HUMANO, HUMANO, HUMANO};
        c.nombres = {"Kirito", "Sukuna", "Tanjiro", "Madara"};
        return c;
    }

    bool esHumano(int indice) const {
        if (indice < 0 || indice >= static_cast<int>(tipos.size()))
            return false;
        return tipos[indice] == HUMANO;
    }

    int cantidadHumanos() const {
        int n = 0;
        for (auto t : tipos) if (t == HUMANO) ++n;
        return n;
    }

    int indiceHumanoLocal() const {
        for (size_t i = 0; i < tipos.size(); ++i)
            if (tipos[i] == HUMANO) return static_cast<int>(i);
        return 0;
    }
};

}