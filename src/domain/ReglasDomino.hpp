#pragma once
#include "Pieza.hpp"
#include "Ficha.hpp"
#include "Jugador.hpp"
#include "Mesa.hpp"
#include <bits/stdc++.h>
using namespace std;

namespace domino {

class ReglasDomino {
public:
    static const int MAX_VALOR = 9;
    static const int TOTAL_FICHAS = (MAX_VALOR + 1) * (MAX_VALOR + 2) / 2;

    static vector<unique_ptr<Pieza>> generarFichas() {
        vector<unique_ptr<Pieza>> fichas;
        for (int i = 0; i <= MAX_VALOR; ++i)
            for (int j = i; j <= MAX_VALOR; ++j)
                fichas.push_back(unique_ptr<Pieza>(new Ficha(i, j)));
        return fichas;
    }

    static void repartirFichas(vector<unique_ptr<Pieza>>& pool,
                                vector<Jugador*>& jugadores,
                                int fichasPorJugador) {
        unsigned seed = static_cast<unsigned>(
            chrono::system_clock::now().time_since_epoch().count());
        shuffle(pool.begin(), pool.end(),
                     default_random_engine(seed));
        for (auto j : jugadores) j->limpiarMano();
        int totalRepartir = fichasPorJugador * static_cast<int>(jugadores.size());
        int idx = 0;
        for (auto& ficha : pool) {
            if (idx < totalRepartir)
                jugadores[idx % jugadores.size()]->agregarFicha(move(ficha));
            ++idx;
        }
        pool.clear();
    }

    static int encontrarPrimerJugador(const vector<Jugador*>& jugadores) {
        int maxPeso = -1;
        int indice = -1;
        for (size_t i = 0; i < jugadores.size(); ++i) {
            for (const auto& f : jugadores[i]->getMano()) {
                Ficha* ficha = dynamic_cast<Ficha*>(f.get());
                if (ficha && ficha->esDoble() && ficha->getExtremoA() > maxPeso) {
                    maxPeso = ficha->getExtremoA();
                    indice = static_cast<int>(i);
                }
            }
        }
        return (indice != -1) ? indice : 0;
    }

    static bool juegoBloqueado(const vector<Jugador*>& jugadores,
                                const Mesa& mesa) {
        if (mesa.vacia()) return false;
        int izq = mesa.getExtremoIzquierdo();
        int der = mesa.getExtremoDerecho();
        for (auto j : jugadores)
            if (j->tieneFicha(izq) || j->tieneFicha(der))
                return false;
        return true;
    }

    static int puntosEnMano(const Jugador& jugador) {
        int suma = 0;
        for (const auto& f : jugador.getMano())
            suma += f->getValorTotal();
        return suma;
    }

    static int obtenerGanadorRonda(const vector<Jugador*>& jugadores) {
        int minPuntos = 1000;
        int ganador = -1;
        for (size_t i = 0; i < jugadores.size(); ++i) {
            int pts = puntosEnMano(*jugadores[i]);
            if (pts < minPuntos) {
                minPuntos = pts;
                ganador = static_cast<int>(i);
            }
        }
        return ganador;
    }

    static bool hayEmpateMinimo(const vector<Jugador*>& jugadores,
                                 int& ganadorIndice) {
        ganadorIndice = -1;
        if (jugadores.empty()) return true;
        int minPuntos = puntosEnMano(*jugadores[0]);
        for (size_t i = 1; i < jugadores.size(); ++i) {
            int pts = puntosEnMano(*jugadores[i]);
            if (pts < minPuntos) minPuntos = pts;
        }
        int cantidadMinimos = 0;
        int ultimoIndiceMinimo = -1;
        for (size_t i = 0; i < jugadores.size(); ++i) {
            if (puntosEnMano(*jugadores[i]) == minPuntos) {
                ++cantidadMinimos;
                ultimoIndiceMinimo = static_cast<int>(i);
            }
        }
        ganadorIndice = ultimoIndiceMinimo;
        return cantidadMinimos > 1;
    }
};

}