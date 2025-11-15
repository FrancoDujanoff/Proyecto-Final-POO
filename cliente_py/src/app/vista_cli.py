"""Interfaz de línea de comandos mínima para el cliente."""

from typing import Optional

from .controlador_cliente import ControladorCliente


class VistaCLI:
    """Vista de consola: lee comandos del usuario y llama al controlador."""

    def __init__(self, controlador: Optional[ControladorCliente] = None) -> None:
        self.c = controlador or ControladorCliente()

    def run(self) -> None:
        """Bucle principal interactivo."""
        print("=== Cliente Robot – CLI ===")
        usr = input("Usuario: ")
        pwd = input("Clave: ")
        self.c.login(usr, pwd)
        print("Login OK. Escriba 'help' para ver comandos, 'exit' para salir.")

        while True:
            cmd = input("> ").strip()
            if cmd in {"exit", "quit"}:
                break
            if cmd == "help":
                self._mostrar_ayuda()
                continue

            try:
                self._dispatch(cmd)
            except Exception as e:  # manejo genérico para empezar
                print(f"ERROR: {e}")

    def _mostrar_ayuda(self) -> None:
        print(
            "Comandos:\n"
            "  subir <archivo>           - Subir archivo G-code\n"
            "  home                      - Ir a home\n"
            "  mover x y z [v]           - Mover a posición (opcionalmente con velocidad)\n"
            "  gripper on|off            - Abrir/cerrar pinza\n"
            "  ejecutar <archivo>        - Ejecutar archivo cargado\n"
            "  rep-op                    - Reporte de operador\n"
            "  rep-log                   - Reporte de log (simple)\n"
            "  rep-admin                 - Reporte administrador (simple)\n"
            "  exit                      - Salir\n"
        )

    def _dispatch(self, cmd: str) -> None:
        toks = cmd.split()
        if not toks:
            return

        op = toks[0]

        if op == "subir" and len(toks) == 2:
            self.c.subir_archivo(toks[1])
            print("OK")
        elif op == "home":
            self.c.home()
            print("OK")
        elif op == "mover" and len(toks) in {4, 5}:
            x, y, z = map(float, toks[1:4])
            v = float(toks[4]) if len(toks) == 5 else None
            self.c.mover(x, y, z, v)
            print("OK")
        elif op == "gripper" and len(toks) == 2:
            self.c.gripper(toks[1].lower() == "on")
            print("OK")
        elif op == "ejecutar" and len(toks) == 2:
            self.c.ejecutar_archivo(toks[1])
            print("OK")
        elif op == "rep-op":
            for l in self.c.reporte_operador():
                print(l)
        elif op == "rep-log":
            for l in self.c.reporte_log({}):
                print(l)
        elif op == "rep-admin":
            for l in self.c.reporte_admin({}):
                print(l)
        else:
            print("Comando desconocido. Use 'help' para ayuda.")
