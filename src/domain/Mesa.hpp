#pragma once
#include "Pieza.hpp"
#include "Ficha.hpp"
#include <bits/stdc++.h>
using namespace std;

namespace domino {

class Mesa {
private:
    deque<unique_ptr<Pieza>> linea;
public:
    void limpiar() { linea.clear(); }
    bool vacia() const { return linea.empty(); }
    size_t size() const { return linea.size(); }

    int getExtremoIzquierdo() const {
        if (linea.empty()) return -1;
        Ficha* f = dynamic_cast<Ficha*>(linea.front().get());
        return f ? f->getExtremoA() : -1;
    }

    int getExtremoDerecho() const {
        if (linea.empty()) return -1;
        Ficha* f = dynamic_cast<Ficha*>(linea.back().get());
        return f ? f->getExtremoB() : -1;
    }

    bool colocarIzquierda(unique_ptr<Pieza> ficha) {
        Ficha* f = dynamic_cast<Ficha*>(ficha.get());
        if (!f) return false;
        if (linea.empty()) return false;
        int izq = getExtremoIzquierdo();
        if (f->getExtremoB() != izq) return false;
        linea.push_front(move(ficha));
        return true;
    }

    bool colocarDerecha(unique_ptr<Pieza> ficha) {
        Ficha* f = dynamic_cast<Ficha*>(ficha.get());
        if (!f) return false;
        if (linea.empty()) return false;
        int der = getExtremoDerecho();
        if (f->getExtremoA() != der) return false;
        linea.push_back(move(ficha));
        return true;
    }

    void colocarInicial(unique_ptr<Pieza> ficha) {
        linea.push_back(move(ficha));
    }

    const deque<unique_ptr<Pieza>>& getLinea() const { return linea; }

    vector<pair<int,int>> getLineaSimple() const {
        vector<pair<int,int>> resultado;
        for (const auto& p : linea) {
            Ficha* f = dynamic_cast<Ficha*>(p.get());
            if (f) resultado.push_back(f->getPar());
        }
        return resultado;
    }

    void cargarLineaSimple(const vector<pair<int,int>>& datos) {
        linea.clear();
        for (const auto& p : datos)
            linea.push_back(unique_ptr<Pieza>(new Ficha(p.first, p.second)));
    }
};

}