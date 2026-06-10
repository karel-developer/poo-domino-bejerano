#pragma once
#include "domain/Pieza.hpp"
#include "domain/Ficha.hpp"
#include "domain/Jugador.hpp"
#include "domain/Mesa.hpp"
#include "domain/ReglasDomino.hpp"
#include "domain/ConfiguracionJuego.hpp"
#include "domain/MotorIA.hpp"
#include <bits/stdc++.h>
using namespace std;

namespace domino {

struct ResultadoTurno {
    bool seJugo;
    bool paso;
    int indiceJugador;
    int fichasRestantes;
    int lado;
    pair<int,int> fichaJugada;

    ResultadoTurno()
        : seJugo(false), paso(false), indiceJugador(-1),
          fichasRestantes(0), lado(0), fichaJugada(-1, -1) {}
};

class JuegoDomino {
public:
    static const int SUR = 0;
    static const int ESTE = 1;
    static const int NORTE = 2;
    static const int OESTE = 3;

private:
    vector<unique_ptr<Jugador>> jugadores;
    Mesa mesa;
    int turnoActual;
    int jugadorSalida;
    bool partidaTerminada;
    bool rondaTerminada;
    bool huboTrancoUltimaRonda;
    int ganadorPartida;
    int ganadorRondaActual;
    set<pair<int,int>> fichasJugadas;
    int rondaNumero;
    int puntosParaGanar;
    int multiplicadorPuntos;
    int fichasPorJugador;
    int fichasRestantesUltimoTurno;
    ConfiguracionJuego config;
    static constexpr int MAX_VALOR = 9;

    void setupNuevaRonda() {
        auto pool = ReglasDomino::generarFichas();
        vector<Jugador*> ptrs;
        for (auto& j : jugadores) ptrs.push_back(j.get());
        ReglasDomino::repartirFichas(pool, ptrs, fichasPorJugador);
        mesa.limpiar();
        fichasJugadas.clear();
        ++rondaNumero;
        rondaTerminada = false;
        ganadorRondaActual = -1;
        partidaTerminada = false;
        fichasRestantesUltimoTurno = -1;
    }

    void configurarJugadoresSegunModo() {
        jugadores.clear();
        for (size_t i = 0; i < config.nombres.size() && i < 4; ++i) {
            jugadores.push_back(unique_ptr<Jugador>(new Jugador(config.nombres[i])));
        }
        while (jugadores.size() < 4) {
            string n = "Jugador " + to_string(jugadores.size() + 1);
            jugadores.push_back(unique_ptr<Jugador>(new Jugador(n)));
        }
    }

public:
    JuegoDomino(const ConfiguracionJuego& configuracion =
                ConfiguracionJuego::solo())
        : turnoActual(0), jugadorSalida(0),
          partidaTerminada(false), rondaTerminada(false),
          huboTrancoUltimaRonda(false),
          ganadorPartida(-1), ganadorRondaActual(-1),
          rondaNumero(0),
          multiplicadorPuntos(1),
          fichasRestantesUltimoTurno(-1),
          config(configuracion) {
        puntosParaGanar = config.puntosParaGanar;
        fichasPorJugador = config.fichasPorJugador;
        configurarJugadoresSegunModo();
        iniciarNuevaRonda();
    }

    void setConfiguracion(const ConfiguracionJuego& nueva) {
        config = nueva;
        puntosParaGanar = config.puntosParaGanar;
        fichasPorJugador = config.fichasPorJugador;
    }

    const ConfiguracionJuego& getConfiguracion() const { return config; }

    void recrearJugadoresSegunConfig() {
        configurarJugadoresSegunModo();
    }

    void setTurnoActual(int t) {
        if (t >= 0 && t < 4) turnoActual = t;
    }
    void setJugadorSalida(int s) {
        if (s >= 0 && s < 4) jugadorSalida = s;
    }
    void setPartidaTerminada(bool b) { partidaTerminada = b; }
    void setRondaTerminada(bool b) { rondaTerminada = b; }
    void setHuboTrancoUltimaRonda(bool b) { huboTrancoUltimaRonda = b; }
    void setMultiplicadorPuntos(int m) {
        if (m >= 1 && m <= 8) multiplicadorPuntos = m;
    }
    void setGanadorPartida(int g) {
        if (g >= -1 && g < 4) ganadorPartida = g;
    }
    void setGanadorRondaActual(int g) {
        if (g >= -1 && g < 4) ganadorRondaActual = g;
    }
    void setRondaNumero(int n) {
        if (n >= 0) rondaNumero = n;
    }
    void setPuntuacionJugador(int idx, int puntos) {
        if (idx >= 0 && idx < static_cast<int>(jugadores.size()))
            jugadores[idx]->setPuntuacion(puntos);
    }
    void setRondasGanadasJugador(int idx, int n) {
        if (idx >= 0 && idx < static_cast<int>(jugadores.size()))
            jugadores[idx]->setRondasGanadas(n);
    }

    void iniciarNuevaRonda() {
        configurarJugadoresSegunModo();
        setupNuevaRonda();

        if (huboTrancoUltimaRonda) {
            huboTrancoUltimaRonda = false;
            jugadorSalida = (jugadorSalida + 1) % static_cast<int>(jugadores.size());
            turnoActual = jugadorSalida;
        } else {
            vector<Jugador*> ptrs;
            for (auto& j : jugadores) ptrs.push_back(j.get());
            jugadorSalida = ReglasDomino::encontrarPrimerJugador(ptrs);
            turnoActual = jugadorSalida;
        }
    }

    bool jugarTurno(int indiceJugador, int fichaSeleccionada, int lado) {
        if (partidaTerminada || rondaTerminada) return false;
        if (indiceJugador != turnoActual) return false;
        if (fichaSeleccionada < 0 ||
            fichaSeleccionada >= jugadores[indiceJugador]->getCantidadFichas())
            return false;
        Pieza* piezaPtr = jugadores[indiceJugador]->getMano()[fichaSeleccionada].get();
        Ficha* fichaPtr = dynamic_cast<Ficha*>(piezaPtr);
        if (!fichaPtr) return false;
        unique_ptr<Pieza> fichaMovida =
            move(jugadores[indiceJugador]->getMano()[fichaSeleccionada]);
        jugadores[indiceJugador]->quitarFicha(fichaSeleccionada);

        if (mesa.vacia()) {
            mesa.colocarInicial(move(fichaMovida));
            Ficha* fichaDyn = dynamic_cast<Ficha*>(mesa.getLinea().back().get());
            if (fichaDyn) {
                int mn = min(fichaDyn->getExtremoA(), fichaDyn->getExtremoB());
                int mx = max(fichaDyn->getExtremoA(), fichaDyn->getExtremoB());
                fichasJugadas.insert(make_pair(mn, mx));
            }
            return true;
        }

        int a = fichaPtr->getExtremoA();
        int b = fichaPtr->getExtremoB();
        int izq = mesa.getExtremoIzquierdo();
        int der = mesa.getExtremoDerecho();
        bool valido = false;
        Ficha* fichaDyn = dynamic_cast<Ficha*>(fichaMovida.get());
        if (lado == 0) {
            if (a == izq || b == izq) {
                if (fichaDyn && a == izq) fichaDyn->girar();
                valido = mesa.colocarIzquierda(move(fichaMovida));
            }
        } else {
            if (a == der || b == der) {
                if (fichaDyn && b == der) fichaDyn->girar();
                valido = mesa.colocarDerecha(move(fichaMovida));
            }
        }
        if (!valido) {
            jugadores[indiceJugador]->agregarFicha(move(fichaMovida));
            return false;
        }
        if (fichaDyn) {
            int mn = min(fichaDyn->getExtremoA(), fichaDyn->getExtremoB());
            int mx = max(fichaDyn->getExtremoA(), fichaDyn->getExtremoB());
            fichasJugadas.insert(make_pair(mn, mx));
        }
        return true;
    }

    bool pasarTurno(int indiceJugador) {
        if (partidaTerminada || rondaTerminada) return false;
        if (indiceJugador != turnoActual) return false;
        if (mesa.vacia()) return false;
        return !puedeJugar(indiceJugador);
    }

    bool puedeJugar(int indiceJugador) const {
        if (indiceJugador < 0 ||
            indiceJugador >= static_cast<int>(jugadores.size()))
            return false;
        if (mesa.vacia()) {
            return jugadores[indiceJugador]->getCantidadFichas() > 0;
        }
        int izq = mesa.getExtremoIzquierdo();
        int der = mesa.getExtremoDerecho();
        return jugadores[indiceJugador]->tieneFichaEnExtremos(izq, der);
    }

    bool puedePasar(int indiceJugador) const {
        if (indiceJugador != turnoActual) return false;
        return !puedeJugar(indiceJugador);
    }

    ResultadoTurno aplicarTurnoIA() {
        ResultadoTurno r;
        if (partidaTerminada || rondaTerminada) return r;
        if (config.esHumano(turnoActual)) return r;
        Jugador& jug = *jugadores[turnoActual];
        DecisionIA dec = MotorIA::instancia().decidir(jug, mesa);
        r.indiceJugador = turnoActual;
        if (dec.pasar) {
            r.seJugo = true;
            r.paso = true;
            r.fichasRestantes = jug.getCantidadFichas();
            return r;
        }
        Ficha* f = dynamic_cast<Ficha*>(jug.getMano()[dec.indiceFicha].get());
        if (!f) return r;
        r.fichaJugada = f->getPar();
        if (jugarTurno(turnoActual, dec.indiceFicha, dec.lado)) {
            r.seJugo = true;
            r.lado = dec.lado;
            r.fichasRestantes = jug.getCantidadFichas();
        }
        return r;
    }

    int getTurnoActual() const { return turnoActual; }
    int getJugadorSalida() const { return jugadorSalida; }
    int getMultiplicadorPuntos() const { return multiplicadorPuntos; }
    bool getHuboTrancoUltimaRonda() const { return huboTrancoUltimaRonda; }
    const Mesa& getMesa() const { return mesa; }
    Mesa& getMesa() { return mesa; }
    const vector<unique_ptr<Jugador>>& getJugadores() const { return jugadores; }
    bool isPartidaTerminada() const { return partidaTerminada; }
    bool isRondaTerminada() const { return rondaTerminada; }
    int getGanadorPartida() const { return ganadorPartida; }
    int getGanadorRondaActual() const { return ganadorRondaActual; }
    int getRondaNumero() const { return rondaNumero; }
    int getPuntosParaGanar() const { return puntosParaGanar; }
    int getFichasPorJugador() const { return fichasPorJugador; }
    static int getMaxValor() { return MAX_VALOR; }

    const set<pair<int,int>>& getFichasJugadas() const { return fichasJugadas; }

    bool esHumano(int indice) const { return config.esHumano(indice); }
    ModoJuego getModo() const { return config.modo; }

    int getJugadorEnPosicionVisual(int posicion) const {
        if (posicion < 0 || posicion > 3) return -1;
        if (config.modo == MODO_SOLO) {
            if (posicion == SUR) return 0;
            if (posicion == ESTE) return 1;
            if (posicion == NORTE) return 2;
            return 3;
        }
        int base = turnoActual;
        return (base + posicion) % 4;
    }

    bool jugadorVisualEsIA(int posicion) const {
        int idx = getJugadorEnPosicionVisual(posicion);
        if (idx < 0) return false;
        return !config.esHumano(idx);
    }

    void siguienteTurno() {
        if (partidaTerminada || rondaTerminada) return;
        turnoActual = (turnoActual + 1) % 4;
    }

    void iniciarNuevaPartida() {
        configurarJugadoresSegunModo();
        for (auto& j : jugadores) {
            j->limpiarMano();
            j->sumarPuntos(-j->getPuntuacion());
        }
        rondaNumero = 0;
        ganadorPartida = -1;
        ganadorRondaActual = -1;
        partidaTerminada = false;
        rondaTerminada = false;
        huboTrancoUltimaRonda = false;
        multiplicadorPuntos = 1;
        jugadorSalida = 0;
        iniciarNuevaRonda();
    }

    void verificarFinRonda() {
        int ganadorRonda = -1;
        for (size_t i = 0; i < jugadores.size(); ++i) {
            if (jugadores[i]->getCantidadFichas() == 0) {
                ganadorRonda = static_cast<int>(i);
                break;
            }
        }
        if (ganadorRonda == -1) {
            vector<Jugador*> ptrs;
            for (auto& j : jugadores) ptrs.push_back(j.get());
            if (ReglasDomino::juegoBloqueado(ptrs, mesa)) {
                int empIndice = -1;
                bool empate = ReglasDomino::hayEmpateMinimo(ptrs, empIndice);
                if (empate) {
                    huboTrancoUltimaRonda = true;
                    multiplicadorPuntos = 2;
                    rondaTerminada = true;
                    ganadorRondaActual = -1;
                    return;
                }
                ganadorRonda = ReglasDomino::obtenerGanadorRonda(ptrs);
            }
        }
        if (ganadorRonda != -1) {
            int puntosObtenidos = 0;
            for (size_t i = 0; i < jugadores.size(); ++i) {
                if (i != static_cast<size_t>(ganadorRonda))
                    puntosObtenidos += ReglasDomino::puntosEnMano(*jugadores[i]);
            }
            puntosObtenidos *= multiplicadorPuntos;
            jugadores[ganadorRonda]->sumarPuntos(puntosObtenidos);
            jugadores[ganadorRonda]->incrementarRondasGanadas();
            ganadorRondaActual = ganadorRonda;
            rondaTerminada = true;
            huboTrancoUltimaRonda = false;
            multiplicadorPuntos = 1;
            if (jugadores[ganadorRonda]->getPuntuacion() >= puntosParaGanar) {
                partidaTerminada = true;
                ganadorPartida = ganadorRonda;
            }
        }
    }

    int getCantidadFichasJugadas() const {
        return static_cast<int>(fichasJugadas.size());
    }

    void setFichasJugadas(const set<pair<int,int>>& s) { fichasJugadas = s; }

    int getFichasRestantesUltimoTurno() const { return fichasRestantesUltimoTurno; }
    void setFichasRestantesUltimoTurno(int n) { fichasRestantesUltimoTurno = n; }
};

}