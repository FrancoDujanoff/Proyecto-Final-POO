"""Presentación de reportes de actividad y log."""

from typing import Any, Dict, Iterable

from ..infraestructura.rpc import ClienteRPC
from ..infraestructura.seguridad import Sesion


class Reportes:
    """Envuelve los métodos RPC de reportes en una interfaz simple."""

    def __init__(self, rpc: ClienteRPC) -> None:
        self.rpc = rpc

    def operador(self, ses: Sesion) -> Iterable[str]:
        """Reporte de actividad del operador actual."""
        return self.rpc.reporte_operador(ses)

    def admin(self, ses: Sesion, filtros: Dict[str, Any]) -> Iterable[str]:
        """Reporte de actividad global (solo admin)."""
        return self.rpc.reporte_admin(ses, filtros)

    def log(self, ses: Sesion, filtros: Dict[str, Any]) -> Iterable[str]:
        """Reporte del log técnico del servidor (solo admin)."""
        return self.rpc.reporte_log(ses, filtros)
