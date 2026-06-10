#include "interfaces/VentanaSFML.hpp"
using namespace domino;

int main() {
    VentanaSFML ventana(1024, 720, "Domino POO - Doble 9 - Clean Architecture");
    ventana.ejecutar();
    return 0;
}
