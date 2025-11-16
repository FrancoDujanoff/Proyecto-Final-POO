# Script de compilación para el servidor C++ en Windows
# Soporta: MinGW (g++), Visual Studio (MSVC), Clang
# Uso: PowerShell -ExecutionPolicy Bypass -File .\build_server.ps1

Write-Host "=== Compilando Servidor RPC en C++ ===" -ForegroundColor Cyan

# Directorio actual
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root

# Directorios
$serverSrc = ".\server\src"
$libDir = ".\lib"
$serverInclude = ".\server\include"
$outputDir = ".\build"
$executable = "$outputDir\servidor_robot.exe"

# Crear directorio de salida si no existe
if (-Not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
    Write-Host "Creada carpeta: $outputDir" -ForegroundColor Green
}

# Auto-detectar compilador disponible
$compiler = $null
$compilerType = $null

# Verificar g++ (MinGW)
if ((Get-Command g++ -ErrorAction SilentlyContinue) -ne $null) {
    $compiler = "g++"
    $compilerType = "MinGW"
    $version = g++ --version | Select-Object -First 1
    Write-Host "Compilador detectado: $version" -ForegroundColor Green
} 
# Verificar cl.exe (MSVC)
elseif ((Get-Command cl.exe -ErrorAction SilentlyContinue) -ne $null) {
    $compiler = "cl.exe"
    $compilerType = "MSVC"
    Write-Host "Compilador detectado: Microsoft Visual C++ (cl.exe)" -ForegroundColor Green
}
# Verificar clang
elseif ((Get-Command clang++ -ErrorAction SilentlyContinue) -ne $null) {
    $compiler = "clang++"
    $compilerType = "Clang"
    $version = clang++ --version | Select-Object -First 1
    Write-Host "Compilador detectado: $version" -ForegroundColor Green
}
else {
    Write-Host "[ERROR] No hay compilador C++ disponible." -ForegroundColor Red
    Write-Host "Opciones:" -ForegroundColor Yellow
    Write-Host "  1. Instala MinGW-w64: https://www.mingw-w64.org/" -ForegroundColor Cyan
    Write-Host "  2. Instala Visual Studio con C++ build tools" -ForegroundColor Cyan
    Write-Host "  3. Instala LLVM/Clang: https://releases.llvm.org/" -ForegroundColor Cyan
    Pop-Location
    exit 1
}

# Lista de archivos .cpp a compilar
$sourceFiles = @(
    "$serverSrc\main.cpp",
    "$serverSrc\AdminCLI.cpp",
    "$serverSrc\BaseDeDatos.cpp",
    "$serverSrc\ControladorGeneral.cpp",
    "$serverSrc\ControladorRobot.cpp",
    "$serverSrc\GestorDeArchivos.cpp",
    "$serverSrc\GestorDeReportes.cpp",
    "$serverSrc\GestorUsuarios.cpp",
    "$serverSrc\MensajeServidor.cpp",
    "$serverSrc\PALogger.cpp",
    "$serverSrc\PuertoSerie.cpp",
    "$serverSrc\Robot.cpp",
    "$serverSrc\ServidorRPC.cpp",
    "$serverSrc\UsuarioServidor.cpp",
    "$libDir\XmlRpcClient.cpp",
    "$libDir\XmlRpcDispatch.cpp",
    "$libDir\XmlRpcServer.cpp",
    "$libDir\XmlRpcServerConnection.cpp",
    "$libDir\XmlRpcServerMethod.cpp",
    "$libDir\XmlRpcSocket.cpp",
    "$libDir\XmlRpcSource.cpp",
    "$libDir\XmlRpcUtil.cpp",
    "$libDir\XmlRpcValue.cpp"
)

# Argumentos de compilación
$includes = "-I`"$serverInclude`" -I`"$libDir`""
$flags = "-std=c++17 -Wall -O2 -pthread"
$libs = "-lws2_32"  # Windows sockets

# Comando de compilación
$compileCmd = "g++ $flags $includes $($sourceFiles -join ' ') -o `"$executable`" $libs"

if ($LASTEXITCODE -eq 0 -and (Test-Path $executable)) {
    Write-Host "`n[OK] Compilacion exitosa!" -ForegroundColor Green
    Write-Host "Ejecutable: $executable" -ForegroundColor Green
    Write-Host "`nPara ejecutar el servidor, usa:" -ForegroundColor Yellow
    Write-Host "  .\build\servidor_robot.exe" -ForegroundColor Cyan
} else {
    Write-Host "`n[ERROR] Fallo en compilacion." -ForegroundColor Red
    exit 1
}

Pop-Location
