"""Configuración del cliente Python (host, puerto, timeouts)."""

from dataclasses import dataclass


@dataclass(frozen=True)
class RPCConfig:
    host: str = "127.0.0.1"
    port: int = 9000
    timeout_s: int = 10

    @property
    def url(self) -> str:
        """URL completa del servidor XML-RPC."""
        return f"http://{self.host}:{self.port}/"
