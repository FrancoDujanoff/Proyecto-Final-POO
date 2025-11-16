# Script de compilacion del servidor C++ en Windows
# Auto-detecta compilador disponible (MinGW/g++, MSVC, Clang)

Write-Host "=== Compilando Servidor RPC en C++ ===" -ForegroundColor Cyan

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root

$serverSrc = ".\server\src"
$libDir = ".\lib"
$serverInclude = ".\server\include"
$outputDir = ".\build"
$executable = "$outputDir\servidor_robot.exe"

if (-Not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
    Write-Host "Carpeta creada: $outputDir" -ForegroundColor Green
}

# Detectar compilador
$compiler = $null
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    $compiler = "g++"
    Write-Host "MinGW detectado: $(g++ --version | Select-Object -First 1)" -ForegroundColor Green
} elseif (Get-Command cl.exe -ErrorAction SilentlyContinue) {
    $compiler = "cl.exe"
    Write-Host "MSVC detectado: cl.exe" -ForegroundColor Green
} elseif (Get-Command clang++ -ErrorAction SilentlyContinue) {
    $compiler = "clang++"
    Write-Host "Clang detectado: $(clang++ --version | Select-Object -First 1)" -ForegroundColor Green
} else {
    Write-Host "[ERROR] Sin compilador C++." -ForegroundColor Red
    Write-Host "Instala: MinGW-w64, Visual Studio, o LLVM/Clang" -ForegroundColor Yellow
    Pop-Location
    exit 1
}

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

$sourceFilesStr = $sourceFiles -join ' '

if ($compiler -eq "g++") {
    $includes = "-I`"$serverInclude`" -I`"$libDir`""
    $flags = "-std=c++17 -Wall -O2 -pthread"
    $cmd = "g++ $flags $includes $sourceFilesStr -o `"$executable`" -lws2_32"
} elseif ($compiler -eq "cl.exe") {
    $includes = "/I`"$serverInclude`" /I`"$libDir`""
    $flags = "/std:c++latest /W4 /O2"
    $cmd = "cl.exe $flags $includes $sourceFilesStr /link ws2_32.lib /OUT:`"$executable`""
} else {
    $includes = "-I`"$serverInclude`" -I`"$libDir`""
    $flags = "-std=c++17 -Wall -O2 -pthread"
    $cmd = "clang++ $flags $includes $sourceFilesStr -o `"$executable`" -lws2_32"
}

Write-Host "Compilando... esto puede tomar minutos..." -ForegroundColor Yellow
Invoke-Expression $cmd 2>&1 | Tee-Object -Variable output | Out-Host

if ($LASTEXITCODE -eq 0 -and (Test-Path $executable)) {
    Write-Host "`n[OK] Compilacion exitosa!" -ForegroundColor Green
    Write-Host "Ejecutable: $executable" -ForegroundColor Green
} else {
    Write-Host "`n[ERROR] Fallo en compilacion." -ForegroundColor Red
    Write-Host "Revisa los errores arriba." -ForegroundColor Red
    Pop-Location
    exit 1
}

Pop-Location
