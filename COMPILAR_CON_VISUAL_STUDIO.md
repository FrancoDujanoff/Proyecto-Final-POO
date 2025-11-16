# COMPILACION CON VISUAL STUDIO - GUIA MANUAL

## Opción 1: Usar Visual Studio IDE (interfaz gráfica)

### Paso 1: Crear nuevo proyecto en Visual Studio

1. Abre Visual Studio
2. **File** → **New** → **Project**
3. Busca: "C++ Console App"
4. Elige la ubicación: `c:\Users\edusa\OneDrive\Escritorio\Proyecto POO\Proyecto-Final-POO\vs_build`
5. Nombre del proyecto: `ServidorRobot`
6. Click **Create**

### Paso 2: Añadir archivos al proyecto

1. En el panel izquierdo (Solution Explorer), verás tu proyecto
2. Haz clic derecho en **Source Files** → **Add** → **Existing Item**
3. Navega a `Proyecto-Final-POO\server\src\` y selecciona TODOS los `.cpp`:
   - main.cpp
   - AdminCLI.cpp
   - BaseDeDatos.cpp
   - ControladorGeneral.cpp
   - ControladorRobot.cpp
   - GestorDeArchivos.cpp
   - GestorDeReportes.cpp
   - GestorUsuarios.cpp
   - MensajeServidor.cpp
   - PALogger.cpp
   - PuertoSerie.cpp
   - Robot.cpp
   - ServidorRPC.cpp
   - UsuarioServidor.cpp

4. Luego en **Header Files** → **Add** → **Existing Item**
5. Navega a `Proyecto-Final-POO\server\include\` y añade TODOS los `.h`

6. Repite para `lib\`:
   - Añade TODOS los `.cpp` a Source Files
   - Mantén los `.h` en Header Files

### Paso 3: Configurar Include Paths

1. Haz clic derecho en el proyecto (ServidorRobot) → **Properties**
2. En la ventana Properties:
   - **Configuration**: selecciona "All Configurations" (arriba a la izquierda)
   - **C/C++** → **General** → **Additional Include Directories**
   - Añade (separadas por `;`):
     ```
     $(ProjectDir)\..\..\server\include;$(ProjectDir)\..\..\lib
     ```
3. Click **Apply** → **OK**

### Paso 4: Configurar Linked Libraries (si es necesario)

1. **Project** → **Properties**
2. **Linker** → **System** → **SubSystem** → selecciona **Console**
3. **Linker** → **Input** → **Additional Dependencies** → añade `ws2_32.lib`
4. Click **Apply** → **OK**

### Paso 5: Copiar usuarios.db

1. Copia el archivo `server/usuarios.db` a la carpeta donde genere el ejecutable:
   - Si compilas en **Debug**: `vs_build\ServidorRobot\Debug\usuarios.db`
   - Si compilas en **Release**: `vs_build\ServidorRobot\Release\usuarios.db`

### Paso 6: Compilar

1. **Build** → **Build Solution** (o presiona Ctrl+Shift+B)
2. Espera a que termine (puede tomar 1-2 minutos)
3. Deberías ver: "Build succeeded" en la ventana Output

### Paso 7: Ejecutar

1. **Debug** → **Start Without Debugging** (o presiona Ctrl+F5)
2. O navega a la carpeta y ejecuta el `.exe` directamente

---

## Opción 2: Usar CMake con Visual Studio (alternativa moderna)

### Paso 1: Instalar CMake

```powershell
# Si tienes Chocolatey:
choco install cmake

# O descarga desde: https://cmake.org/download/
```

### Paso 2: Crear CMakeLists.txt

Crea un archivo llamado `CMakeLists.txt` en la raíz del proyecto con este contenido:

```cmake
cmake_minimum_required(VERSION 3.15)
project(ServidorRobot CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir directorios
include_directories(server/include lib)

# Archivos fuente del servidor
set(SERVER_SOURCES
    server/src/main.cpp
    server/src/AdminCLI.cpp
    server/src/BaseDeDatos.cpp
    server/src/ControladorGeneral.cpp
    server/src/ControladorRobot.cpp
    server/src/GestorDeArchivos.cpp
    server/src/GestorDeReportes.cpp
    server/src/GestorUsuarios.cpp
    server/src/MensajeServidor.cpp
    server/src/PALogger.cpp
    server/src/PuertoSerie.cpp
    server/src/Robot.cpp
    server/src/ServidorRPC.cpp
    server/src/UsuarioServidor.cpp
)

# Archivos fuente de XML-RPC
set(XMLRPC_SOURCES
    lib/XmlRpcClient.cpp
    lib/XmlRpcDispatch.cpp
    lib/XmlRpcServer.cpp
    lib/XmlRpcServerConnection.cpp
    lib/XmlRpcServerMethod.cpp
    lib/XmlRpcSocket.cpp
    lib/XmlRpcSource.cpp
    lib/XmlRpcUtil.cpp
    lib/XmlRpcValue.cpp
)

# Crear ejecutable
add_executable(servidor_robot ${SERVER_SOURCES} ${XMLRPC_SOURCES})

# Linkear bibliotecas de Windows
if(WIN32)
    target_link_libraries(servidor_robot ws2_32)
endif()

# Copiar usuarios.db a carpeta de compilacion
add_custom_command(TARGET servidor_robot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/server/usuarios.db
    $<TARGET_FILE_DIR:servidor_robot>/usuarios.db
)
```

### Paso 3: Compilar con CMake

```powershell
cd "c:\Users\edusa\OneDrive\Escritorio\Proyecto POO\Proyecto-Final-POO"

# Crear carpeta de compilacion
mkdir cmake_build
cd cmake_build

# Generar archivos de Visual Studio
cmake .. -G "Visual Studio 17 2022"

# O si tienes Visual Studio 2019:
cmake .. -G "Visual Studio 16 2019"

# Compilar
cmake --build . --config Release

# El ejecutable esta en: Release\servidor_robot.exe
```

---

## Opción 3: Script PowerShell Auto (si encuentras la ruta de VS)

Si ya sabes donde esta instalado Visual Studio, puedes modificar el script `build_visual_studio.ps1`:

1. Abre `build_visual_studio.ps1`
2. Busca la seccion `$vsVersions = @(`
3. Modifica las rutas segun tu Visual Studio:
   - Abre cmd y ejecuta: `dir "C:\Program Files*" /B` para ver que hay
   - Busca una carpeta similar a: `Microsoft Visual Studio\2022\...`
   - La ruta debe terminar en: `\VC\Auxiliary\Build\vcvars64.bat`

4. Guarda el archivo y ejecuta:
```powershell
PowerShell -ExecutionPolicy Bypass -File .\build_visual_studio.ps1
```

---

## Resumen Rapido

| Opcion | Tiempo | Dificultad | Ventaja |
|--------|--------|-----------|---------|
| IDE Visual Studio | 5-10 min | Facil | Interfaz visual, debugging integrado |
| CMake | 3-5 min | Media | Moderno, reutilizable |
| Script PowerShell | 1-2 min | Media | Automatico (si funciona) |

Mi recomendacion: **CMake** es la forma mas moderna y limpia. **IDE Visual Studio** si prefieres interfaz visual.

---

## Si nada funciona

1. Verifica que tienes C++ build tools instalado:
   - Visual Studio Installer → Modify → selecciona "Desktop development with C++"
   
2. Busca manualmente donde esta vcvars64.bat:
   ```powershell
   Get-ChildItem -Path "C:\Program Files*" -Recurse -Filter "vcvars64.bat" -ErrorAction SilentlyContinue
   ```

3. Una vez que encuentres la ruta, modificala en `build_visual_studio.ps1`

---

Dime cual opcion prefieres y te ayudo a implementarla!
