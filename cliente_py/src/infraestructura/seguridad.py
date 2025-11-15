"""Manejo de sesión/autenticación en el cliente."""

from dataclasses import dataclass
from typing import Optional


class AutorizacionDenegada(Exception):
    """Se lanza cuando el servidor rechaza las credenciales o permisos."""
    pass


@dataclass
class Sesion:
    usuario: str
    token: Optional[str] = None   # si el servidor maneja tokens
    es_admin: bool = False

    def assert_activa(self) -> None:
        """Verifica que la sesión esté inicializada (precondición)."""
        if not self.usuario:
            raise AutorizacionDenegada("Sesión no inicializada")
