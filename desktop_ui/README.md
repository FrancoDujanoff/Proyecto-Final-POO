Desktop UI - Cliente y Admin
===========================

Este directorio contiene un prototipo de interfaces de escritorio (Tkinter):

- `operator_ui.py`: Interfaz para el operador (conectar, enviar G-Code, estado, logs).
- `admin_ui.py`: Interfaz para administrar usuarios localmente en `server/usuarios.db`.
- `rpc_adapter.py`: Adaptador que encapsula llamadas XML-RPC y manejo de token.

Requisitos
---------
- Python 3.8+
- Tkinter (incluido en las instalaciones estándar de Python en Windows)

Uso rápido
---------
1. Inicia el mock server (si quieres probar sin compilar C++):

```powershell
python .\cliente\tools\mock_servidor.py
```

2. En otra terminal, inicia la UI del operador:

```powershell
python .\desktop_ui\operator_ui.py
```

3. Para administrar usuarios (modifica `server/usuarios.db` localmente):

```powershell
python .\desktop_ui\admin_ui.py
```

Notas
-----
- El Admin UI modifica `server/usuarios.db` directamente (mecanismo `crear_local`).
- La UI de operador utiliza las llamadas del Mock Server (métodos: `conectar`, `desconectar`, `enviar_comando_gcode`, `obtener_estado_robot`, `obtener_historial_logs`).
- Para usar el servidor C++ real, ajusta host/port en la UI y/o compila el servidor.
 
Función-primera (Factory) API
----------------------------
Si prefieres crear la interfaz desde una función (por ejemplo para integrarla en
otra app), hay builders disponibles:

- `desktop_ui.operator_ui.build_operator_frame(parent, rpc_adapter=None)` -> devuelve un `ttk.Frame` listo para usar.
- `desktop_ui.operator_ui.create_operator_window(rpc_adapter=None)` -> crea una `Tk` con la UI y la devuelve (llamar `.mainloop()`).
- `desktop_ui.admin_ui.build_admin_frame(parent, repo_root=None)` -> devuelve un `ttk.Frame` con la UI de administración.
- `desktop_ui.admin_ui.create_admin_window(repo_root=None)` -> crea una `Tk` con la UI de administración.

Ejemplo minimal para integrar sin crear terminal adicional:

```python
import tkinter as tk
from desktop_ui.operator_ui import create_operator_window

win = create_operator_window()
win.mainloop()
```
