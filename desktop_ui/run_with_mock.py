"""Ejecutor rápido que arranca el Mock Server en un hilo y abre la UI.

Esto evita tener que abrir una terminal separada: el servidor mock se ejecuta
en un hilo dentro del mismo proceso, y la ventana Tkinter será visible.

Uso:
    python desktop_ui\run_with_mock.py

Nota: Para pruebas locales solamente. No es una solución de producción.
"""
import threading
import time
import tkinter as tk
import sys
from pathlib import Path

# Asegurar que la raíz del proyecto esté en sys.path para poder importar
# el paquete `cliente` incluso si este script se ejecuta directamente desde
# el subdirectorio `desktop_ui`.
repo_root = Path(__file__).resolve().parents[1]
if str(repo_root) not in sys.path:
    sys.path.insert(0, str(repo_root))

from cliente.tools import mock_servidor
from desktop_ui.operator_ui import create_operator_window


def _start_mock():
    try:
        mock_servidor.iniciar_servidor()
    except Exception as e:
        print("Mock server terminó con excepción:", e)


def main():
    t = threading.Thread(target=_start_mock, daemon=True, name="MockServerThread")
    t.start()

    # Damos un pequeño retraso para que el servidor esté escuchando antes de
    # abrir la UI; en la práctica la UI puede reintentar o el adaptador manejar
    # la reconexión.
    time.sleep(0.5)

    win = create_operator_window()
    win.mainloop()


if __name__ == '__main__':
    main()
