"""Gestión local de archivos del cliente (validación y lectura)."""

from pathlib import Path


class ArchivoInvalido(Exception):
    """Se lanza cuando el archivo no existe o no es válido."""
    pass


class GestorArchivosCliente:
    """Se encarga de acceder a los archivos G-code del lado cliente."""

    def __init__(self, base: Path | str = "./gcode") -> None:
        self.base = Path(base)
        self.base.mkdir(parents=True, exist_ok=True)

    def leer_binario(self, nombre: str) -> bytes:
        """Lee un archivo binario desde la carpeta de trabajo."""
        p = (self.base / nombre).resolve()
        if not p.is_file():
            raise ArchivoInvalido(f"No existe: {p}")
        return p.read_bytes()

    def listar(self) -> list[str]:
        """Lista los archivos disponibles en la carpeta base."""
        return sorted(x.name for x in self.base.glob("*") if x.is_file())
