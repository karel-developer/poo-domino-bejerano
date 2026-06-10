#pragma once
#include <string>
#include <vector>
using namespace std;

namespace domino {

struct SlideTutorial {
    string titulo;
    vector<string> parrafos;
    string ejemplo;
};

class Tutorial {
public:
    static vector<SlideTutorial> obtenerSlides() {
        vector<SlideTutorial> s;

        SlideTutorial s1;
        s1.titulo = "Bienvenido al Domino Doble-9";
        s1.parrafos = {
            "El domino doble-9 se juega con 55 fichas",
            "numeradas del 0|0 al 9|9.",
            "Juegan 4 participantes. Cada uno recibe 10",
            "fichas. Las 15 restantes quedan fuera del juego."
        };
        s1.ejemplo = "Fichas: 0|0, 0|1, 0|2 ... 9|8, 9|9";
        s.push_back(s1);

        SlideTutorial s2;
        s2.titulo = "Objetivo del juego";
        s2.parrafos = {
            "Gana la ronda el primer jugador que se",
            "quede sin fichas en la mano.",
            "Gana la partida el primero en alcanzar",
            "100 puntos acumulados."
        };
        s2.ejemplo = "Meta: 100 puntos para ganar la partida";
        s.push_back(s2);

        SlideTutorial s3;
        s3.titulo = "Como jugar";
        s3.parrafos = {
            "En tu turno, coloca una ficha en uno de",
            "los extremos de la cadena haciendo coincidir",
            "el numero de tu ficha con el numero visible",
            "del extremo de la cadena."
        };
        s3.ejemplo = "Si la cadena termina en 5, puedes jugar 5|3 o 5|5";
        s.push_back(s3);

        SlideTutorial s5;
        s5.titulo = "Cuando no puedes jugar";
        s5.parrafos = {
            "Si no tienes ninguna ficha que coincida",
            "con los extremos, debes Pasar el turno.",
            "Si TODOS los jugadores pasan consecutivamente",
            "la ronda se TRANCA."
        };
        s5.ejemplo = "Boton 'Pasar' aparece solo si no tienes jugables";
        s.push_back(s5);

        SlideTutorial s6;
        s6.titulo = "Tranque y puntos dobles";
        s6.parrafos = {
            "En el tranque, si hay empate en la menor",
            "cantidad de puntos en mano, nadie gana.",
            "Quien salio pierde el derecho a salir y el",
            "siguiente jugador vale sus puntos al DOBLE (x2)."
        };
        s6.ejemplo = "Si un jugador suma 30 puntos, con x2 obtiene 60";
        s.push_back(s6);

        SlideTutorial s7;
        s7.titulo = "Modos de juego";
        s7.parrafos = {
            "Modo SOLO: tu contra 3 computadoras.",
            "Las fichas de la IA permanecen ocultas.",
            "",
            "Modo 4 JUGADORES: todos humanos en el mismo",
            "dispositivo. La perspectiva rota para que",
            "solo veas tu mano en tu turno."
        };
        s7.ejemplo = "Elige modo en el menu principal";
        s.push_back(s7);

        SlideTutorial s8;
        s8.titulo = "Listo para jugar!";
        s8.parrafos = {
            "Pulsa 'Menu' para volver al menu principal",
            "y elegir tu modo de juego favorito.",
            "",
            "Divertete!"
        };
        s8.ejemplo = "Suerte!";
        s.push_back(s8);

        return s;
    }
};

}