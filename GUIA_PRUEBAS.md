# Guía de Pruebas: Proyecto Servidor-Cliente RPC

## Resumen del Proyecto

Este es un proyecto de control de robot con arquitectura cliente-servidor:

- **Servidor C++** (`server/`): Controla un robot real vía puerto serie, expone una API XML-RPC en puerto `8080`, gestiona usuarios y autorización.
- **Mock Server Python** (`cliente/tools/mock_servidor.py`): Simulador del servidor para pruebas locales rápidas en puerto `8000`.
- **Cliente Python** (`cliente/src/cliente_robot.py`): Cliente demo que se conecta vía XML-RPC, autentica y realiza operaciones (enviar G-Code, obtener estado, logs).
- **Cliente Python 2** (`cliente_py/src/infraestructura/rpc.py`): Otra implementación de cliente con arquitectura más estructurada, también usa XML-RPC.
- **Tests Automatizados** (`cliente/tests/`, `cliente_py/tests/`): pytest para validar integración sin compilar C++.

### Componentes Principales

#### Server (C++)
- `ServidorRPC`: Expone métodos XML-RPC con prefijo `robot.*` (ej. `robot.login`, `robot.mover`).
- `ControladorGeneral`: Orquesta la lógica (gestión de usuarios, conexión robot, movimientos).
- `GestorUsuarios`: Maneja autenticación, lee de `server/usuarios.db` (formato CSV: `usuario,contraseña,esAdmin(1|0)`).
- `GestorDeArchivos`, `GestorDeReportes`: Manejo de archivos G-code y reportes.
- `Robot`, `PuertoSerie`: Control del hardware robot.
- `PALogger`: Logging de actividades.

#### Cliente (Python)
- `ClienteRobot`: Conecta vía `xmlrpc.client.ServerProxy`, maneja token de sesión.
- Métodos principales: `conectar(usr, pwd)`, `desconectar()`, `enviar_gcode()`, `get_estado()`, `get_logs()`.

#### Mock Server (Python)
- `ApiRobotMock`: Implementa API simulada sin acceso a puerto serie ni archivos.
- Métodos: `conectar()`, `desconectar()`, `enviar_comando_gcode()`, `obtener_estado_robot()`, `obtener_historial_logs()`.
- Útil para tests rápidos sin compilar C++.

---

## Pasos para Probar (Opción 1: Mock Server - MÁS RÁPIDO)

### Requisito Previo
- Python 3.8+ instalado.
- pytest instalado: `pip install pytest`

### Paso 1: Abrir dos terminales PowerShell

**Terminal 1: Iniciar Mock Server**
```powershell
Set-Location "c:\Users\edusa\OneDrive\Escritorio\Proyecto POO\Proyecto-Final-POO"
python .\cliente\tools\mock_servidor.py
```

Deberías ver:
```
Instancia de ApiRobotMock creada.
Usuarios válidos: {'admin': '1234'}
Iniciando Mock Server en http://localhost:8000/RPC2 ...
Servidor listo. Esperando peticiones (Ctrl+C para salir).
```

### Paso 2: En Terminal 2, ejecutar pruebas rápidas

**Opción A: Prueba simple de conectividad**
```powershell
Set-Location "c:\Users\edusa\OneDrive\Escritorio\Proyecto POO\Proyecto-Final-POO"
python .\cliente\tools\probar_conexion.py
```

Esperado: Token recibido en `localhost:8000` y error en `localhost:8080` (servidor C++ no está corriendo).

**Opción B: Ejecutar el cliente demo**
```powershell
python .\cliente\src\cliente_robot.py
```

Esperado: Múltiples pruebas internas (conexión exitosa, envío de G-Code, estado del robot, logs).

**Opción C: Ejecutar tests automatizados (recomendado)**
```powershell
python -m pytest -q cliente/tests/test_cliente.py -q
```

Esperado: 4-5 tests PASS.

---

## Pasos para Probar (Opción 2: Servidor C++ - MÁS COMPLETO)

### Requisitos Previos

1. **Compilador C++**: 
   - Visual Studio 2019+ (con C++ build tools), O
   - MinGW-w64 (g++), O
   - LLVM/Clang

2. **Archivo de usuarios**:
   - Ya existe `server/usuarios.db` con contenido: `admin,admin123,1`
   - Usuario: `admin`, Contraseña: `admin123`, Rol: Admin

### Método A: Compilar con g++ (MinGW)

1. **Verificar que g++ esté disponible**:
   ```powershell
   g++ --version
   ```
   Si NO está instalado, descarga [MinGW-w64](https://www.mingw-w64.org/) e instala.

2. **Compilar el servidor**:
   ```powershell
   Set-Location "c:\Users\edusa\OneDrive\Escritorio\Proyecto POO\Proyecto-Final-POO"
   PowerShell -ExecutionPolicy Bypass -File .\build_server.ps1
   ```

   Esto debería crear `build/servidor_robot.exe`.

3. **Ejecutar el servidor**:
   ```powershell
   .\build\servidor_robot.exe
   ```

   Esperado: el servidor se inicia, abre una CLI administrativa donde puedes ingresar comandos.

### Método B: Compilar con Visual Studio (Visual C++)

1. Abre Visual Studio.
2. Crea un nuevo proyecto "C++ Console App".
3. Añade todos los archivos:
   - `server/src/*.cpp`
   - `lib/*.cpp` (solo los `.cpp`, NO los `.o`)
4. En Propiedades del Proyecto → C/C++ → General → Include Directories, añade:
   - `server/include`
   - `lib`
5. Copia `server/usuarios.db` a la carpeta donde Visual Studio genera el `.exe` (ej. `Debug/`).
6. Compilar (Ctrl+Shift+B) y ejecutar (Ctrl+F5).

### Método C: Compilar con CMake (recomendado para sistemas grandes)

Si prefieres usar CMake (aún no existe `CMakeLists.txt` en el repo, pero es fácil crearlo):
```powershell
# Crear directorio de compilación
mkdir build
cd build
cmake .. -G "MinGW Makefiles"  # o "Visual Studio 16 2019"
cmake --build .
.\servidor_robot.exe
```

### Paso 2: Con el servidor C++ corriendo, probar conexión

En otra terminal:
```powershell
python .\cliente\tools\probar_conexion.py
```

Esperado: Ambas pruebas (`:8000` y `:8080`) responden correctamente.

### Paso 3: Ejecutar cliente contra servidor C++

**Modificar cliente para usar el servidor C++ real:**

Si quieres que `cliente_robot.py` se conecte al servidor C++ (puerto 8080), puedes:
- Editar `cliente/src/cliente_robot.py`, línea 10-12:
  ```python
  def __init__(self, host='localhost', puerto=8080):  # Cambiar puerto de 8000 a 8080
      url_servidor = f"http://{host}:{puerto}/RPC2"
  ```
- O crear una instancia con parámetros: `cliente = ClienteRobot(puerto=8080)`

Luego ejecutar:
```powershell
python .\cliente\src\cliente_robot.py
```

---

## Ejecución Completa de Tests

Para validar toda la integración:

```powershell
# Tests del cliente (con mock server)
python -m pytest cliente/tests/test_cliente.py -v

# Tests del cliente_py
python -m pytest cliente_py/tests/ -v

# Todos los tests
python -m pytest cliente/tests/ cliente_py/tests/ -v
```

---

## Información de Usuarios

### Usuario por defecto (ya cargado en `server/usuarios.db`):
- **Usuario**: `admin`
- **Contraseña**: `admin123`
- **Rol**: Administrador

### En el Mock Server (hardcodeado):
- **Usuario**: `admin`
- **Contraseña**: `1234`
- **Rol**: Administrador

**Nota**: Las credenciales del mock y del servidor C++ son distintas por diseño. El mock usa `admin/1234` para las pruebas rápidas; el servidor C++ espera `admin/admin123` (o lo que esté en `usuarios.db`).

---

## Conexiones y Puertos

| Servicio | Host | Puerto | Path |
|----------|------|--------|------|
| Mock Server (Python) | localhost | 8000 | /RPC2 |
| Servidor C++ | localhost | 8080 | /RPC2 |

---

## Troubleshooting

| Problema | Causa | Solución |
|----------|-------|----------|
| `ConnectionRefusedError` en `probar_conexion.py` | Servidor no corre en ese puerto. | Asegúrate de iniciar mock (`mock_servidor.py`) o servidor C++. |
| `g++ no encontrado` en compilación | MinGW no instalado. | Descarga e instala MinGW-w64 o usa Visual Studio. |
| `ModuleNotFoundError: No module named 'cliente'` en tests | Python path incorrecto. | Ejecuta desde la raíz del repo; pytest auto-descubre. |
| `pytest: El término no se reconoce` | pytest no en PATH. | Usa `python -m pytest` en lugar de `pytest`. |
| Usuario o contraseña incorrectos | Credenciales no coinciden con servidor. | Para mock: `admin/1234`; para C++: `admin/admin123` (o según `usuarios.db`). |

---

## Arquitectura Simplificada

```
[Cliente Python]
       ↓ XML-RPC (HTTP)
[Servidor XML-RPC]
       ↓
[ControladorGeneral]
       ↓
[GestorUsuarios] [GestorDeArchivos] [ControladorRobot]
       ↓                ↓                   ↓
[usuarios.db]    [archivos G-Code]  [PuertoSerie → Robot Físico]
```

---

## Siguientes Pasos (Opcionales)

1. **Implementar método `crear_usuario` en `GestorUsuarios`**: Actualmente la CLI permite crear usuarios, pero no hay endpoint XML-RPC.
2. **Añadir método `obtener_reportes` en API**: El servidor tiene `GestorDeReportes` pero no se expone vía RPC.
3. **Mejorar seguridad**: Usar hash de contraseña en lugar de texto plano en `usuarios.db`.
4. **Logs persistentes**: La API retorna logs simulados; integrar con PALogger para logs reales.
5. **Tests de integración contra servidor C++**: Una vez compilado, escribir tests que se conecten a `:8080`.
