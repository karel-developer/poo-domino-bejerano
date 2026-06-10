@echo off
REM Build script alternativo (CMD) para Domino POO
REM Uso: build.cmd

setlocal
cd /d "%~dp0"

echo.
echo >> Domino POO - Build Script (CMD)
echo.

REM 1) Detectar g++
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    if exist "C:\msys64\mingw64\bin\g++.exe" set "PATH=C:\msys64\mingw64\bin;%PATH%"
)
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERR] g++ no encontrado. Instala MSYS2 UCRT64 + gcc.
    exit /b 1
)

REM 2) Detectar SFML
set "SFML_INC=C:\SFML-src\SFML-2.6.2\include"
set "SFML_LIB=C:\SFML-src\SFML-2.6.2\build\lib"
if not exist "%SFML_INC%\SFML\Graphics.hpp" (
    if exist "C:\SFML\include\SFML\Graphics.hpp" (
        set "SFML_INC=C:\SFML\include"
        set "SFML_LIB=C:\SFML\lib"
    )
)
if not exist "%SFML_INC%\SFML\Graphics.hpp" (
    echo [ERR] SFML 2.x no encontrado.
    exit /b 1
)

REM 3) Compilar
echo >> Compilando src\main.cpp...
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic ^
    src\main.cpp ^
    -I src -I libs -I "%SFML_INC%" ^
    -L "%SFML_LIB%" ^
    -o domino.exe ^
    -lsfml-graphics -lsfml-window -lsfml-system
if %ERRORLEVEL% neq 0 (
    echo [ERR] Compilacion fallo.
    exit /b 1
)

REM 4) Copiar DLLs
copy /Y "%SFML_LIB%\sfml-graphics-2.dll" . >nul
copy /Y "%SFML_LIB%\sfml-window-2.dll"   . >nul
copy /Y "%SFML_LIB%\sfml-system-2.dll"   . >nul

if not exist "datos" mkdir datos
if not exist "datos\partida.json" echo {}> "datos\partida.json"

echo.
echo BUILD COMPLETO: domino.exe listo
endlocal
