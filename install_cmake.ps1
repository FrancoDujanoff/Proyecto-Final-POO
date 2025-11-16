# Script para instalar CMake automaticamente en Windows

Write-Host "=== Instalando CMake ===" -ForegroundColor Cyan

# URL del installer de CMake (version estable)
$cmakeUrl = "https://github.com/Kitware/CMake/releases/download/v3.27.8/cmake-3.27.8-windows-x86_64.msi"
$installerPath = "$env:TEMP\cmake-installer.msi"

Write-Host "Descargando CMake..." -ForegroundColor Yellow
try {
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $cmakeUrl -OutFile $installerPath -UseBasicParsing
    Write-Host "Descarga completada" -ForegroundColor Green
} catch {
    Write-Host "Error descargando CMake: $_" -ForegroundColor Red
    Write-Host "Descargalo manualmente desde: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

Write-Host "`nInstalando CMake (esto puede tomar un minuto)..." -ForegroundColor Yellow
# Instalar silenciosamente y añadir a PATH
& msiexec.exe /i $installerPath /qn /norestart ADD_CMAKE_TO_PATH=System 2>&1 | Out-Null

# Esperar a que se complete
Start-Sleep -Seconds 3

# Verificar instalacion
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    Write-Host "`n[OK] CMake instalado correctamente!" -ForegroundColor Green
    cmake --version | Select-Object -First 1
} else {
    Write-Host "`n[ERROR] No se pudo instalar CMake." -ForegroundColor Red
    Write-Host "Instalo manual desde: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

# Limpiar installer
Remove-Item -Force $installerPath -ErrorAction SilentlyContinue

Write-Host "`nAhora ejecuta:" -ForegroundColor Cyan
Write-Host "  PowerShell -ExecutionPolicy Bypass -File .\build_cmake.ps1" -ForegroundColor Yellow
