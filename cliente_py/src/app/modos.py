"""Comandos de alto nivel para modo manual y automático."""

from typing import Optional

from ..infraestructura.rpc import ClienteRPC
from ..infraestructura.seguridad import Sesion


class ModosOperacion:
    """Envuelve las operaciones de movimiento / ejecución de archivo."""

    def __init__(self, rpc: ClienteRPC) -> None:
        self.rpc = rpc

    def set_modo(self, ses: Sesion, manual: bool, absoluto: bool) -> None:
        """Selecciona modo manual/automático y absoluto/relativo."""
        self.rpc.seleccionar_modo(ses, manual, absoluto)

    def home(self, ses: Sesion) -> None:
        """Envía el robot a la posición home."""
        self.rpc.home(ses)

    def mover(
        self,
        ses: Sesion,
        x: float,
        y: float,
        z: float,
        v: Optional[float] = None,
    ) -> None:
        """Mueve el robot a la posición indicada."""
        self.rpc.mover(ses, x, y, z, v)

    def gripper(self, ses: Sesion, on: bool) -> None:
        """Abre o cierra la pinza."""
        self.rpc.gripper(ses, on)

    def ejecutar_archivo(self, ses: Sesion, nombre: str) -> None:
        """Ejecuta un archivo G-code almacenado en el servidor."""
        self.rpc.ejecutar_archivo(ses, nombre)
