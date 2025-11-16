# Script para descargar e instalar MinGW-w64 (g++) automaticamente

Write-Host "=== Instalando MinGW-w64 (g++) ===" -ForegroundColor Cyan

# URL del MinGW-w64 portable (más rápido que el instalador)
$mingwUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/12.2.0-rt_v10-rev2/x86_64-12.2.0-release-win32-seh-rt_v10-rev2.7z"
$extractPath = "C:\MinGW"
$sevenZipExe = "C:\Program Files\7-Zip\7z.exe"

Write-Host "Descargando MinGW-w64 (esto puede tomar unos minutos)..." -ForegroundColor Yellow

$downloadPath = "$env:TEMP\mingw.7z"
try {
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $mingwUrl -OutFile $downloadPath -UseBasicParsing -MaximumRetryCount 3
    Write-Host "Descarga completada" -ForegroundColor Green
} catch {
    Write-Host "Error descargando MinGW: $_" -ForegroundColor Red
    Write-Host "`nAlternativa: Descargalo manualmente desde:" -ForegroundColor Yellow
    Write-Host "  https://www.mingw-w64.org/downloads/" -ForegroundColor Cyan
    Write-Host "  O usa: https://github.com/niXman/mingw-builds-binaries/releases" -ForegroundColor Cyan
    exit 1
}

# Verificar si 7-Zip está disponible
if (-Not (Test-Path $sevenZipExe)) {
    Write-Host "`n[ERROR] 7-Zip no encontrado en: $sevenZipExe" -ForegroundColor Red
    Write-Host "Instalalo desde: https://www.7-zip.org/download.html" -ForegroundColor Yellow
    Write-Host "O extrae manualmente el archivo: $downloadPath" -ForegroundColor Yellow
    exit 1
}

Write-Host "`nExtrayendo MinGW (esto puede tomar 1-2 minutos)..." -ForegroundColor Yellow

# Crear directorio si no existe
if (-Not (Test-Path $extractPath)) {
    New-Item -ItemType Directory -Path $extractPath | Out-Null
}

# Extraer
& $sevenZipExe x $downloadPath -o"$extractPath" -y 2>&1 | Out-Null

if ($LASTEXITCODE -eq 0) {
    Write-Host "Extraccion completada" -ForegroundColor Green
} else {
    Write-Host "[ERROR] Fallo en extraccion" -ForegroundColor Red
    exit 1
}

# Añadir a PATH
$mingwBin = "$extractPath\mingw64\bin"
$currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")

if ($currentPath -notlike "*$mingwBin*") {
    Write-Host "`nAñadiendo MinGW a PATH..." -ForegroundColor Yellow
    [Environment]::SetEnvironmentVariable("PATH", "$currentPath;$mingwBin", "User")
    Write-Host "PATH actualizado" -ForegroundColor Green
}

# Limpiar descarga
Remove-Item -Force $downloadPath -ErrorAction SilentlyContinue

Write-Host "`n[OK] MinGW-w64 instalado en: $extractPath" -ForegroundColor Green

Write-Host "`nAhora necesitas CERRAR y ABRIR PowerShell de nuevo para que cargue el PATH." -ForegroundColor Yellow
Write-Host "Luego ejecuta:" -ForegroundColor Cyan
Write-Host "  PowerShell -ExecutionPolicy Bypass -File .\build_server.ps1" -ForegroundColor Yellow

Write-Host "`nO verifica que g++ esta disponible:" -ForegroundColor Cyan
Write-Host "  g++ --version" -ForegroundColor Yellow
