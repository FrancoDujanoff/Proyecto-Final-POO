# Script para compilar con CMake + Visual Studio
# Auto-detecta Visual Studio y genera build files

Write-Host "=== Compilacion con CMake + Visual Studio ===" -ForegroundColor Cyan

# Verificar que CMake esta disponible
if (-Not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "[ERROR] CMake no encontrado. Instalalo desde: https://cmake.org/download/" -ForegroundColor Red
    exit 1
}

Write-Host "CMake detectado: $(cmake --version | Select-Object -First 1)" -ForegroundColor Green

# Cambiar a directorio del proyecto
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root

# Crear carpeta de compilacion
$buildDir = ".\build_cmake"
if (-Not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
    Write-Host "Carpeta creada: $buildDir" -ForegroundColor Green
}

# Detectar version de Visual Studio
Write-Host "`nBuscando Visual Studio..." -ForegroundColor Cyan
$vsVersions = @(
    ("Visual Studio 17 2022", "2022"),
    ("Visual Studio 16 2019", "2019")
)

$generator = $null
foreach ($vs in $vsVersions) {
    $genName = $vs[0]
    $year = $vs[1]
    Write-Host "Intentando: $genName" -ForegroundColor Yellow
    
    # Intenta generar con este generator
    $result = & cmake -G $genName -S . -B $buildDir 2>&1
    if ($LASTEXITCODE -eq 0) {
        $generator = $genName
        Write-Host "Exito con: $generator" -ForegroundColor Green
        break
    }
}

if (-Not $generator) {
    Write-Host "[ERROR] No se encontro Visual Studio compatible." -ForegroundColor Red
    Write-Host "Opciones:" -ForegroundColor Yellow
    Write-Host "  1. Instala Visual Studio 2022 Community (gratuito)" -ForegroundColor Cyan
    Write-Host "  2. Instala CMake" -ForegroundColor Cyan
    Write-Host "  3. USA directamente la IDE de Visual Studio (ver COMPILAR_CON_VISUAL_STUDIO.md)" -ForegroundColor Cyan
    Pop-Location
    exit 1
}

# Compilar
Write-Host "`nCompilando en modo Release..." -ForegroundColor Yellow
$compileResult = & cmake --build $buildDir --config Release 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n[OK] Compilacion exitosa!" -ForegroundColor Green
    
    # Buscar el ejecutable
    $exe = Get-ChildItem -Path $buildDir -Recurse -Filter "servidor_robot.exe" | Select-Object -First 1
    if ($exe) {
        Write-Host "Ejecutable: $($exe.FullName)" -ForegroundColor Green
        Write-Host "`nPara ejecutar:" -ForegroundColor Cyan
        Write-Host "  $($exe.FullName)" -ForegroundColor Yellow
    } else {
        Write-Host "Nota: Busca servidor_robot.exe en: $buildDir" -ForegroundColor Yellow
    }
} else {
    Write-Host "`n[ERROR] Fallo en compilacion." -ForegroundColor Red
    Write-Host $compileResult
    Pop-Location
    exit 1
}

Pop-Location
