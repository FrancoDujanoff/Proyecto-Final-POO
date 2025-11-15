"""Objeto de dominio para intercambio Cliente⇄Servidor.

Se hace responsable de su serialización a un diccionario,
que luego puede viajar por XML-RPC, JSON, etc.
"""

from dataclasses import dataclass
from typing import Any, Dict


@dataclass
class Mensaje:
    tipo: str          # "peticion" | "respuesta" | "error"
    usuario: str
    payload: Dict[str, Any]

    def to_wire(self) -> Dict[str, Any]:
        """Serializa a un diccionario transmisible."""
        return {
            "tipo": self.tipo,
            "usuario": self.usuario,
            "payload": self.payload,
        }

    @classmethod
    def from_wire(cls, data: Dict[str, Any]) -> "Mensaje":
        """Crea un Mensaje a partir del diccionario recibido."""
        if not {"tipo", "usuario", "payload"}.issubset(data):
            raise ValueError("Mensaje incompleto")
        return cls(
            tipo=str(data["tipo"]),
            usuario=str(data["usuario"]),
            payload=dict(data["payload"]),
        )
