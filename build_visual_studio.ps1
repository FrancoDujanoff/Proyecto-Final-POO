# Script de compilacion para Visual Studio en Windows
# Detecta automaticamente la instalacion de Visual Studio y compila el proyecto

Write-Host "=== Buscando Visual Studio ===" -ForegroundColor Cyan

# Rutas donde puede estar Visual Studio (versiones recientes)
$vsVersions = @(
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
)

$vcvarsPath = $null
foreach ($path in $vsVersions) {
    if (Test-Path $path) {
        $vcvarsPath = $path
        Write-Host "Visual Studio encontrado: $path" -ForegroundColor Green
        break
    }
}

if (-Not $vcvarsPath) {
    Write-Host "[ERROR] Visual Studio no encontrado en rutas estandar." -ForegroundColor Red
    Write-Host "Asegurate de que Visual Studio 2019+ este instalado con C++ build tools." -ForegroundColor Yellow
    exit 1
}

# Cambiar a directorio del proyecto
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root

Write-Host "`n=== Configurando entorno de Visual Studio ===" -ForegroundColor Cyan

# Crear script temporal que ejecuta vcvars y luego cl.exe
$tempScript = "$env:TEMP\compile_temp.bat"
@"
@echo off
REM Inicializar variables de entorno de Visual Studio
call "$vcvarsPath"

REM Directorio de proyecto
cd /d "$root"

REM Crear directorio de compilacion
if not exist build mkdir build

REM Definir archivos fuente
set SOURCES=^
  server\src\main.cpp ^
  server\src\AdminCLI.cpp ^
  server\src\BaseDeDatos.cpp ^
  server\src\ControladorGeneral.cpp ^
  server\src\ControladorRobot.cpp ^
  server\src\GestorDeArchivos.cpp ^
  server\src\GestorDeReportes.cpp ^
  server\src\GestorUsuarios.cpp ^
  server\src\MensajeServidor.cpp ^
  server\src\PALogger.cpp ^
  server\src\PuertoSerie.cpp ^
  server\src\Robot.cpp ^
  server\src\ServidorRPC.cpp ^
  server\src\UsuarioServidor.cpp ^
  lib\XmlRpcClient.cpp ^
  lib\XmlRpcDispatch.cpp ^
  lib\XmlRpcServer.cpp ^
  lib\XmlRpcServerConnection.cpp ^
  lib\XmlRpcServerMethod.cpp ^
  lib\XmlRpcSocket.cpp ^
  lib\XmlRpcSource.cpp ^
  lib\XmlRpcUtil.cpp ^
  lib\XmlRpcValue.cpp

REM Compilar
echo Compilando... esto puede tomar unos minutos...
cl.exe /std:c++latest /W4 /O2 /I"server\include" /I"lib" /EHsc %SOURCES% /link ws2_32.lib /OUT:build\servidor_robot.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [OK] Compilacion exitosa!
    echo Ejecutable creado: build\servidor_robot.exe
) else (
    echo.
    echo [ERROR] Fallo en compilacion.
    exit /b 1
)
"@ | Out-File -FilePath $tempScript -Encoding ASCII

Write-Host "Ejecutando compilacion..." -ForegroundColor Yellow

# Ejecutar el script temporal
cmd /c $tempScript

# Limpiar archivo temporal
Remove-Item -Force $tempScript -ErrorAction SilentlyContinue

if (Test-Path "build\servidor_robot.exe") {
    Write-Host "`n[OK] Compilacion completada correctamente!" -ForegroundColor Green
    Write-Host "Ejecutable: build\servidor_robot.exe" -ForegroundColor Green
    Write-Host "`nPara ejecutar el servidor:" -ForegroundColor Cyan
    Write-Host "  .\build\servidor_robot.exe" -ForegroundColor Yellow
} else {
    Write-Host "`n[ERROR] El ejecutable no se creo. Revisa los errores arriba." -ForegroundColor Red
    Pop-Location
    exit 1
}

Pop-Location
