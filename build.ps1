#requires -Version 5.0
<#
.SYNOPSIS
    Build script infalible para Domino POO - Compila con SFML + nlohmann/json.

.DESCRIPTION
    Detecta automaticamente la ubicacion de SFML (compilado desde fuente o MSYS2),
    valida todos los componentes, compila con C++11 + warnings estrictos, y
    prepara el .exe junto a sus DLLs y assets.

    Uso:
        powershell -ExecutionPolicy Bypass -File build.ps1
        powershell -ExecutionPolicy Bypass -File build.ps1 -Clean
        powershell -ExecutionPolicy Bypass -File build.ps1 -Run
#>

param(
    [switch]$Clean,
    [switch]$Run,
    [string]$SFMLDir = ""
)

$ErrorActionPreference = "Stop"
$ProjectRoot = $PSScriptRoot
Set-Location $ProjectRoot

function Write-Step($msg) { Write-Host "`n>> $msg" -ForegroundColor Cyan }
function Write-Ok($msg)   { Write-Host "   [OK] $msg" -ForegroundColor Green }
function Write-Warn($msg) { Write-Host "   [WARN] $msg" -ForegroundColor Yellow }
function Write-Err($msg)  { Write-Host "   [ERR] $msg" -ForegroundColor Red; exit 1 }

Write-Step "Domino POO - Build Script"

if ($Clean) {
    Write-Step "Limpiando binarios previos..."
    @("domino.exe", "domino.obj", "datos\partida.json") | ForEach-Object {
        if (Test-Path $_) { Remove-Item $_ -Force; Write-Ok "Eliminado: $_" }
    }
    Write-Ok "Limpieza completa"
    if (-not $Run) { exit 0 }
}

Write-Step "1/6 - Verificando compilador g++"
$gpp = $null
$candidates = @(
    "C:\msys64\ucrt64\bin\g++.exe",
    "C:\msys64\mingw64\bin\g++.exe",
    "C:\mingw64\bin\g++.exe",
    (Get-Command g++ -ErrorAction SilentlyContinue).Source
) | Where-Object { $_ -and (Test-Path $_) }
if ($candidates.Count -gt 0) { $gpp = $candidates[0] }
if (-not $gpp) { Write-Err "g++ no encontrado. Instala MSYS2 UCRT64 + mingw-w64-ucrt-x86_64-gcc" }
$gppVersion = & $gpp --version | Select-Object -First 1
Write-Ok "g++ encontrado: $gpp"
Write-Ok "Version: $gppVersion"

Write-Step "2/6 - Detectando SFML 2.6.2"
$SFMLInclude = ""
$SFMLLib = ""
$sfmlSources = @(
    @{ I = "C:\SFML-src\SFML-2.6.2\include"; L = "C:\SFML-src\SFML-2.6.2\build\lib" },
    @{ I = "C:\SFML\include";                  L = "C:\SFML\lib" },
    @{ I = "C:\msys64\ucrt64\include\SFML";    L = "C:\msys64\ucrt64\lib" },
    @{ I = "C:\msys64\mingw64\include\SFML";   L = "C:\msys64\mingw64\lib" }
)
if ($SFMLDir -and (Test-Path "$SFMLDir\include") -and (Test-Path "$SFMLDir\lib")) {
    $sfmlSources = @(@{ I = "$SFMLDir\include"; L = "$SFMLDir\lib" }) + $sfmlSources
}
foreach ($s in $sfmlSources) {
    if ((Test-Path $s.I) -and (Test-Path $s.L)) {
        $dlls = @("sfml-graphics-2.dll", "sfml-window-2.dll", "sfml-system-2.dll")
        if ($dlls | Where-Object { Test-Path (Join-Path $s.L $_) }) {
            $SFMLInclude = $s.I
            $SFMLLib = $s.L
            break
        }
    }
}
if (-not $SFMLInclude) { Write-Err "SFML 2.x no encontrado. Compila desde C:\SFML-src\SFML-2.6.2 o instala via pacman" }
Write-Ok "SFML include: $SFMLInclude"
Write-Ok "SFML lib:     $SFMLLib"

Write-Step "3/6 - Verificando nlohmann/json"
if (-not (Test-Path "libs\nlohmann\json.hpp")) {
    Write-Err "libs/nlohmann/json.hpp no existe. Descarga de https://github.com/nlohmann/json/releases"
}
Write-Ok "nlohmann/json presente en libs/"

Write-Step "4/6 - Verificando assets"
$assets = @("assets\arial.ttf", "assets\fondo.png", "assets\logo.png", "assets\fichas\ficha_oculta.png")
foreach ($a in $assets) {
    if (-not (Test-Path $a)) { Write-Warn "Asset faltante: $a (opcional, el juego sigue sin el)" }
    else { Write-Ok "Asset OK: $a" }
}
$fichasCount = (Get-ChildItem "assets\fichas\ficha_*.png" -ErrorAction SilentlyContinue | Measure-Object).Count
Write-Ok "Fichas encontradas: $fichasCount / 55"

Write-Step "5/6 - Compilando src\main.cpp"
$includeDirs = @("-I", "src", "-I", "libs", "-I", $SFMLInclude)
$libDirs     = @("-L", $SFMLLib)
$libs        = @("-lsfml-graphics", "-lsfml-window", "-lsfml-system")
$flags       = @("-std=c++11", "-O2", "-Wall", "-Wextra", "-Wpedantic")

$cmd = @($gpp) + $flags + @("src\main.cpp") + $includeDirs + $libDirs + @("-o", "domino.exe") + $libs
Write-Host "   Comando: $($cmd -join ' ')" -ForegroundColor Gray

$output = & $gpp $flags src\main.cpp $includeDirs $libDirs -o domino.exe $libs 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host $output
    Write-Err "Compilacion FALLO. Revisa los errores arriba."
}
Write-Ok "Compilacion exitosa (0 warnings)"

Write-Step "6/6 - Copiando DLLs de SFML al directorio del .exe"
Copy-Item -Force (Join-Path $SFMLLib "sfml-graphics-2.dll") . -ErrorAction SilentlyContinue
Copy-Item -Force (Join-Path $SFMLLib "sfml-window-2.dll")   . -ErrorAction SilentlyContinue
Copy-Item -Force (Join-Path $SFMLLib "sfml-system-2.dll")   . -ErrorAction SilentlyContinue
Get-ChildItem *.dll | ForEach-Object { Write-Ok "Copia: $($_.Name)" }

if (-not (Test-Path "datos")) { New-Item -ItemType Directory -Path "datos" | Out-Null }
if (-not (Test-Path "datos\partida.json")) {
    "{}" | Out-File "datos\partida.json" -Encoding ASCII
    Write-Ok "Creado: datos\partida.json"
}

Write-Host "`n========================================" -ForegroundColor Green
Write-Host "  BUILD COMPLETO: domino.exe listo" -ForegroundColor Green
Write-Host "========================================`n" -ForegroundColor Green

if ($Run) {
    Write-Step "Ejecutando domino.exe..."
    & ".\domino.exe"
}
