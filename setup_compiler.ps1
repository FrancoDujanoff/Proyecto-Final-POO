# Script SIMPLIFICADO: Descargar e instalar MinGW-w64 sin dependencias externas

Write-Host "=== Setup: Instalando compilador g++ (MinGW-w64) ===" -ForegroundColor Cyan

$mingwPath = "C:\mingw64"

# Si ya existe, saltamos instalacion
if (Test-Path "$mingwPath\bin\g++.exe") {
    Write-Host "[OK] MinGW ya esta instalado en: $mingwPath" -ForegroundColor Green
    Write-Host "Versión: $(& "$mingwPath\bin\g++.exe" --version | Select-Object -First 1)" -ForegroundColor Green
} else {
    Write-Host "`nPara compilar el servidor C++ necesitas un compilador." -ForegroundColor Yellow
    Write-Host "`nOPCIONES:" -ForegroundColor Cyan
    Write-Host "`n1. Descargar MinGW-w64 (recomendado):" -ForegroundColor Green
    Write-Host "   - Ve a: https://www.mingw-w64.org/downloads/" -ForegroundColor Yellow
    Write-Host "   - Descarga: x86_64-posix-seh (Online installer)" -ForegroundColor Yellow
    Write-Host "   - Instala en: C:\mingw64" -ForegroundColor Yellow
    Write-Host "   - Cierra y abre PowerShell de nuevo" -ForegroundColor Yellow
    Write-Host "   - Luego ejecuta este script de nuevo" -ForegroundColor Yellow
    
    Write-Host "`n2. O usa Compilación Rapida sin servidor C++:" -ForegroundColor Green
    Write-Host "   - Solo necesitas Python" -ForegroundColor Yellow
    Write-Host "   - Ejecuta: python -m pytest -q cliente/tests/test_cliente.py -q" -ForegroundColor Yellow
    
    exit 1
}

# Si llegamos aqui, el compilador ya esta listo
Write-Host "`n[OK] Compilador listo!" -ForegroundColor Green
Write-Host "`nAhora puedes compilar:" -ForegroundColor Cyan
Write-Host "  PowerShell -ExecutionPolicy Bypass -File .\build_server.ps1" -ForegroundColor Yellow
