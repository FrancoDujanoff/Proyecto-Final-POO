"""Logger local simple para el cliente (solo en memoria)."""

from datetime import datetime
from typing import Iterable, List


class LoggerCliente:
    """Logger muy simple: guarda líneas en memoria."""

    def __init__(self) -> None:
        self._lineas: List[str] = []

    def log(self, nivel: str, modulo: str, mensaje: str) -> None:
        """Agrega una entrada de log con timestamp."""
        ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self._lineas.append(f"{ts} | {nivel.upper():7} | {modulo} | {mensaje}")

    def dump(self) -> Iterable[str]:
        """Devuelve las líneas de log acumuladas."""
        return list(self._lineas)
