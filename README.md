# Domino POO - Doble-9 (55 fichas) - 4 Jugadores

Juego de domino doble-9 con **Clean Architecture**, **3 modos de juego**, **tutorial interactivo**, **IA integrada** y persistencia JSON. Proyecto academico - Examen Parcial de Programacion Orientada a Objetos (Universidad de Pinar del Rio).

## Requisitos del sistema

| Componente | Version | Descarga |
|---|---|---|
| **Compilador** | GCC (C++11) | [MSYS2 UCRT64](https://www.msys2.org/) - `pacman -S mingw-w64-ucrt-x86_64-gcc` |
| **SFML** | 2.6.2 | [Compilar desde fuente](https://www.sfml-dev.org/download/sfml/2.6.2/) o `pacman -S mingw-w64-ucrt-x86_64-sfml` |
| **nlohmann/json** | 3.11.3 | [Descargar json.hpp](https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp) y colocarlo en `libs/nlohmann/` |
| **Assets** | — | Incluidos en el repositorio (`assets/`) |

### Instalacion paso a paso (Windows)

1. **Instalar MSYS2** desde [msys2.org](https://www.msys2.org/)
2. Abrir "MSYS2 UCRT64" y ejecutar:
   ```bash
   pacman -Syu                           # Actualizar paquetes
   pacman -S mingw-w64-ucrt-x86_64-gcc   # GCC
   pacman -S mingw-w64-ucrt-x86_64-sfml  # SFML (via pacman)
   ```
3. Copiar `json.hpp` en la carpeta `libs/nlohmann/` del proyecto
4. Compilar con uno de los scripts abajo

> **Alternativa**: si prefieres compilar SFML manualmente, descarga el [codigo fuente 2.6.2](https://www.sfml-dev.org/download/sfml/2.6.2/), descomprimo en `C:\SFML-src\SFML-2.6.2`, compilo con CMake y los scripts lo detectan automaticamente.

### Instalacion paso a paso (Linux)

```bash
sudo apt update
sudo apt install g++ libsfml-dev       # Debian/Ubuntu
# o: sudo pacman -S gcc sfml            # Arch
mkdir -p libs/nlohmann
wget -O libs/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
```

## Compilacion

### Windows (recomendado: un solo comando)

**PowerShell:**
```powershell
powershell -ExecutionPolicy Bypass -File build.ps1
```

Flags opcionales:
- `build.ps1 -Clean` — limpia binarios previos
- `build.ps1 -Run` — compila y ejecuta

**CMD:**
```cmd
build.cmd
```
o
```cmd
build.bat
```

### Linux
```bash
chmod +x build.sh
./build.sh
```

### Compilacion manual

**Windows (MSYS2/MinGW):**
```bash
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic \
    src/main.cpp \
    -I src -I libs -I <RUTA_SFML>/include \
    -L <RUTA_SFML>/lib \
    -o domino.exe \
    -lsfml-graphics -lsfml-window -lsfml-system
# Copiar DLLs (solo Windows):
# cp <RUTA_SFML>/lib/sfml-graphics-2.dll .
# cp <RUTA_SFML>/lib/sfml-window-2.dll .
# cp <RUTA_SFML>/lib/sfml-system-2.dll .
```

**Linux:**
```bash
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic \
    src/main.cpp \
    -I src -I libs \
    -o domino \
    -lsfml-graphics -lsfml-window -lsfml-system
```

## Clean Architecture (capas)

```
src/
├── domain/                       # CAPA 1: Dominio puro (sin I/O, sin UI, sin JSON)
│   ├── Pieza.hpp                 # Clase abstracta
│   ├── Ficha.hpp                 # Pieza concreta (0|0 .. 9|9)
│   ├── Jugador.hpp               # Entidad jugador
│   ├── Mesa.hpp                  # Linea de juego
│   ├── ReglasDomino.hpp          # Reglas (repartir, bloquear, ganador)
│   ├── ConfiguracionJuego.hpp    # Modos, tipos de jugador, metas
│   └── MotorIA.hpp               # IA con 3 niveles
│
├── application/                  # CAPA 2: Casos de uso (orquesta el dominio)
│   └── JuegoDomino.hpp           # Control de partida, turnos, fin de ronda
│
├── infrastructure/               # CAPA 3: Adaptadores externos
│   └── PersistenciaPartidaJson.hpp  # Serializacion JSON (nlohmann)
│
├── interfaces/                   # CAPA 4: UI (adaptadores de entrada)
│   ├── Tutorial.hpp              # 8 slides del tutorial
│   └── VentanaSFML.hpp           # Ventana principal con SFML (~1800 lineas)
│
└── main.cpp                      # Composition root (entry point)
```

**Regla de dependencia** (Clean Architecture):
```
interfaces ──> application ──> domain
infrastructure ──> application ──> domain
```

- `domain` no depende de nadie (puro C++/STL)
- `application` solo conoce `domain`
- `infrastructure` solo conoce `application` y `domain` (para guardar/cargar)
- `interfaces` solo conoce `application` y `domain` (para dibujar)
- `main.cpp` es el composition root: ensambla todas las capas

**Namespace**: Todas las clases estan dentro de `namespace domino` para evitar colisiones.

## Cumplimiento de los requisitos del examen

| Requisito | Cumplimiento |
|---|---|
| C++11 minimo, compilacion limpia sin warnings | `g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic` -> 0 warnings |
| SFML para interfaz grafica | SFML 2.6.2, ventana redimensionable, ciclo de eventos |
| Arquitectura por capas (Clean Architecture) | 4 capas estrictas: `domain` / `application` / `infrastructure` / `interfaces` |
| Persistencia en .json | `PersistenciaPartidaJson` en `infrastructure/`, nlohmann/json header-only |
| STL: al menos 3 de vector, set, map | 8+ contenedores (ver tabla mas abajo) |
| POO: herencia, polimorfismo, encapsulamiento, abstraccion | `Pieza` (abstracta) -> `Ficha` con `std::unique_ptr<Pieza>` polimorfico |
| Gestion de memoria sin fugas | Solo `std::unique_ptr`, `std::move`, RAII |
| Repositorio: `poo-domino-bejerano` | Este README documenta el proyecto |

## Modos de juego

Al iniciar aparece un menu con 4 opciones:

### 1. Tutorial
7 diapositivas que explican reglas, salida, dobles, tranco y modos.

### 2. Jugar Solo
- Tu (J1) contra 3 IAs
- Las fichas de la IA permanecen ocultas
- La IA juega con un delay de 1 segundo

### 3. 4 Jugadores
- 4 humanos en el mismo dispositivo
- La perspectiva rota; aparece un overlay "Pasar el dispositivo" entre turnos

### 4. Salir
Cierra el juego.

## Reglas del juego

- **Modalidad**: doble-9, 55 fichas (0|0 a 9|9)
- **Reparto**: 10 por jugador, 15 fuera de juego
- **Sin pozo**: pasar cuando no hay jugables
- **Reglas normales** se aplican a partir del segundo turno en adelante
- **Meta**: 100 puntos para ganar la partida
- **Tranco**: juego bloqueado + empate -> ronda nula, siguiente vale x2

## Inteligencia artificial

3 niveles:
- **FACIL**: juega cualquier ficha jugable al azar
- **NORMAL** (default): prefiere no-dobles, mayor valor primero
- **DIFICIL**: intenta NO jugar dobles; si obligado, mayor valor

## Estructura completa del proyecto

```
.
|-- assets/
|   |-- arial.ttf
|   |-- fondo.png
|   |-- logo.png
|   `-- fichas/
|       |-- ficha_0_0.png ... ficha_9_9.png
|       `-- ficha_oculta.png
|-- datos/
|   `-- partida.json
|-- libs/
|   `-- nlohmann/
|       `-- json.hpp
|-- src/
|   |-- main.cpp
|   |-- domain/
|   |   |-- Pieza.hpp
|   |   |-- Ficha.hpp
|   |   |-- Jugador.hpp
|   |   |-- Mesa.hpp
|   |   |-- ReglasDomino.hpp
|   |   |-- ConfiguracionJuego.hpp
|   |   `-- MotorIA.hpp
|   |-- application/
|   |   `-- JuegoDomino.hpp
|   |-- infrastructure/
|   |   `-- PersistenciaPartidaJson.hpp
|   `-- interfaces/
|       |-- Tutorial.hpp
|       `-- VentanaSFML.hpp
|-- build.ps1                      Script PowerShell (recomendado)
|-- build.cmd                      Script CMD (alternativa)
|-- README.md                      Este archivo
`-- domino.exe                     Binario generado
```

## Contenedores STL utilizados (8, requisito era 3)

| Contenedor | Uso | Capa |
|---|---|---|
| `std::vector` | Mano del jugador, texturas | domain, interfaces |
| `std::deque` | Linea de la mesa (insercion O(1) en ambos extremos) | domain |
| `std::set` | Fichas unicas jugadas, `getFichasJugadas()` | application |
| `std::map` | Texturas SFML por par (a,b) | interfaces |
| `std::pair<int,int>` | Ficha simplificada para JSON, clave en set | domain, application |
| `std::unique_ptr` | Posesion de Pieza, Jugador, JuegoDomino | todas |
| `std::string` | Nombres, mensajes, paths de assets | todas |
| `nlohmann::json` | Persistencia | infrastructure |

## POO aplicado

- **Herencia**: `Pieza` (abstracta) -> `Ficha` (concreta)
- **Polimorfismo**: almacenamiento en `std::vector<std::unique_ptr<Pieza>>` con `dynamic_cast<Ficha*>`
- **Encapsulamiento**: `private`/`public` estrictos en todas las clases
- **Abstraccion**: clases abstractas (`Pieza`), interfaces implicitas (`IRepositorio` por convencion)

## Interfaz de usuario

- Tutorial (8 slides con navegacion por teclado/boton)
- Menu principal (4 opciones)
- Overlay "Pasar dispositivo" (modo 4 jugadores)
- Pantalla de juego:
  - Cadena central con layout multi-fila y bordes
  - Mano del jugador abajo con highlighting (gris/verde/amarillo)
  - 3 paneles de oponentes (norte/este/oeste) con fichas ocultas
  - Indicador TURNO + meta + multiplicador
  - Botonera inferior contextual (Pasar, <-Izq, Der->, Jugar, NuevaRnd, Menu)
- Overlay de fin de ronda / fin de partida

## Controles

- **Click** en fichas y botones
- **ESC** durante juego -> vuelve al menu
- **Flechas** / **Space** en el tutorial

## Solucion de problemas

| Problema | Solucion |
|---|---|
| `g++` no encontrado (Windows) | Instala MSYS2 UCRT64 + `pacman -S mingw-w64-ucrt-x86_64-gcc` |
| `g++` no encontrado (Linux) | `sudo apt install g++` (Debian/Ubuntu) o `sudo pacman -S gcc` (Arch) |
| `SFML/Graphics.hpp` no encontrado (Windows) | `pacman -S mingw-w64-ucrt-x86_64-sfml` O compila desde [fuente](https://www.sfml-dev.org/download/sfml/2.6.2/) en `C:\SFML-src\SFML-2.6.2`; los scripts lo detectan automaticamente |
| `SFML/Graphics.hpp` no encontrado (Linux) | `sudo apt install libsfml-dev` |
| `libs/nlohmann/json.hpp` no encontrado | Descarga de [github.com/nlohmann/json](https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp) y colocalo en `libs/nlohmann/` |
| DLLs no encontradas al ejecutar (Windows) | El script `build.ps1` las copia automaticamente; si no, copialas manualmente desde `<RUTA_SFML>/lib/` |
| Ventana negra al iniciar | Verifica que `assets/fondo.png` existe; el juego usa color verde de fallback |
| Las fichas se ven grises | Falta `assets/fichas/*.png`; ver carpeta de assets |
| `./build.sh` no tiene permisos (Linux) | `chmod +x build.sh` |
