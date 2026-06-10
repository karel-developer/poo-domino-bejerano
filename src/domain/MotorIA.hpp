#pragma once
#include "Pieza.hpp"
#include "Ficha.hpp"
#include "Jugador.hpp"
#include "Mesa.hpp"
#include <bits/stdc++.h>
using namespace std;

namespace domino {

struct DecisionIA {
    bool pasar;
    int indiceFicha;
    int lado;

    DecisionIA() : pasar(false), indiceFicha(-1), lado(0) {}
    DecisionIA(bool p, int i, int l) : pasar(p), indiceFicha(i), lado(l) {}
};

class MotorIA {
public:
    enum Dificultad { FACIL, NORMAL, DIFICIL };

private:
    Dificultad dificultad;
    MotorIA() : dificultad(NORMAL) {}

    static bool puedeColocarseEn(const Ficha& f, int extremoMesa) {
        return f.getExtremoA() == extremoMesa || f.getExtremoB() == extremoMesa;
    }

    static int valorFicha(const Ficha& f) {
        return f.getValorTotal();
    }

public:
    static MotorIA& instancia() {
        static MotorIA unica;
        return unica;
    }

    void setDificultad(Dificultad d) { dificultad = d; }
    Dificultad getDificultad() const { return dificultad; }

    DecisionIA decidir(const Jugador& jugador, const Mesa& mesa) const {
        const auto& mano = jugador.getMano();
        int izq = mesa.getExtremoIzquierdo();
        int der = mesa.getExtremoDerecho();
        bool mesaVacia = mesa.vacia();

        if (mesaVacia) {
            int mejorIdx = 0;
            int mejorPeso = -1;
            for (size_t i = 0; i < mano.size(); ++i) {
                Ficha* f = dynamic_cast<Ficha*>(mano[i].get());
                if (!f) continue;
                int val = valorFicha(*f);
                int peso = (f->esDoble() ? 1000 : 0) + val;
                if (peso > mejorPeso) {
                    mejorPeso = peso;
                    mejorIdx = static_cast<int>(i);
                }
            }
            if (mejorPeso < 0) return DecisionIA(true, -1, 0);
            return DecisionIA(false, mejorIdx, 1);
        }

        vector<pair<int,int>> jugablesIzq;
        vector<pair<int,int>> jugablesDer;
        vector<int> indices;

        for (size_t i = 0; i < mano.size(); ++i) {
            Ficha* f = dynamic_cast<Ficha*>(mano[i].get());
            if (!f) continue;
            bool pIzq = puedeColocarseEn(*f, izq);
            bool pDer = puedeColocarseEn(*f, der);
            if (pIzq) jugablesIzq.emplace_back(static_cast<int>(i), valorFicha(*f));
            if (pDer) jugablesDer.emplace_back(static_cast<int>(i), valorFicha(*f));
        }

        if (jugablesIzq.empty() && jugablesDer.empty())
            return DecisionIA(true, -1, 0);

        if (dificultad == FACIL) {
            if (!jugablesDer.empty()) {
                int idx = static_cast<int>((rand() % jugablesDer.size()));
                return DecisionIA(false, jugablesDer[idx].first, 1);
            }
            int idx = static_cast<int>((rand() % jugablesIzq.size()));
            return DecisionIA(false, jugablesIzq[idx].first, 0);
        }

        auto mejorNoDoble = [](vector<pair<int,int>>& v) {
            int mejor = -1;
            int mejorVal = -1;
            for (size_t i = 0; i < v.size(); ++i) {
                if (v[i].second > mejorVal) {
                    mejorVal = v[i].second;
                    mejor = static_cast<int>(i);
                }
            }
            return mejor;
        };
        auto mejorCualquiera = [](vector<pair<int,int>>& v) {
            int mejor = -1;
            int mejorVal = -1;
            for (size_t i = 0; i < v.size(); ++i) {
                if (v[i].second > mejorVal) {
                    mejorVal = v[i].second;
                    mejor = static_cast<int>(i);
                }
            }
            return mejor;
        };

        if (dificultad == DIFICIL) {
            for (size_t i = 0; i < mano.size(); ++i) {
                Ficha* f = dynamic_cast<Ficha*>(mano[i].get());
                if (!f) continue;
                if (f->esDoble()) continue;
                if (puedeColocarseEn(*f, izq))
                    return DecisionIA(false, static_cast<int>(i), 0);
                if (puedeColocarseEn(*f, der))
                    return DecisionIA(false, static_cast<int>(i), 1);
            }
            int idxIzq = mejorCualquiera(jugablesIzq);
            if (idxIzq >= 0) return DecisionIA(false, jugablesIzq[idxIzq].first, 0);
            int idxDer = mejorCualquiera(jugablesDer);
            return DecisionIA(false, jugablesDer[idxDer].first, 1);
        }

        bool hayNoDobleIzq = false, hayNoDobleDer = false;
        for (auto& p : jugablesIzq) {
            Ficha* f = dynamic_cast<Ficha*>(mano[p.first].get());
            if (f && !f->esDoble()) { hayNoDobleIzq = true; break; }
        }
        for (auto& p : jugablesDer) {
            Ficha* f = dynamic_cast<Ficha*>(mano[p.first].get());
            if (f && !f->esDoble()) { hayNoDobleDer = true; break; }
        }

        if (hayNoDobleDer) {
            int mejor = -1, mejorVal = -1;
            for (auto& p : jugablesDer) {
                Ficha* f = dynamic_cast<Ficha*>(mano[p.first].get());
                if (f && !f->esDoble() && p.second > mejorVal) {
                    mejorVal = p.second;
                    mejor = p.first;
                }
            }
            return DecisionIA(false, mejor, 1);
        }
        if (hayNoDobleIzq) {
            int mejor = -1, mejorVal = -1;
            for (auto& p : jugablesIzq) {
                Ficha* f = dynamic_cast<Ficha*>(mano[p.first].get());
                if (f && !f->esDoble() && p.second > mejorVal) {
                    mejorVal = p.second;
                    mejor = p.first;
                }
            }
            return DecisionIA(false, mejor, 0);
        }
        if (!jugablesDer.empty()) {
            int idx = mejorNoDoble(jugablesDer);
            return DecisionIA(false, jugablesDer[idx].first, 1);
        }
        int idx = mejorNoDoble(jugablesIzq);
        return DecisionIA(false, jugablesIzq[idx].first, 0);
    }
};

}