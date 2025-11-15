"""Controlador de orquestación entre Vista, RPC y dominio."""

from typing import Optional, Dict, Any, List

from ..infraestructura.rpc import ClienteRPC
from ..infraestructura.seguridad import Sesion
from .logger_cliente import LoggerCliente
from .archivos import GestorArchivosCliente
from .modos import ModosOperacion
from .reportes import Reportes


class ControladorCliente:
    """
    Capa de aplicación que coordina:
        Vista  ⇄  ControladorCliente  ⇄  RPC / Archivos / Modos / Reportes
    """

    def __init__(self, rpc: Optional[ClienteRPC] = None, logger: Optional[LoggerCliente] = None) -> None:
        self.rpc = rpc or ClienteRPC()
        self.logger = logger or LoggerCliente()
        self.sesion: Optional[Sesion] = None
        self.archivos = GestorArchivosCliente()
        self.modos = ModosOperacion(self.rpc)
        self.reportes = Reportes(self.rpc)

    # --- Utilidad interna ---

    def _req_sesion(self) -> Sesion:
        """Devuelve la sesión activa o lanza error si no hay login."""
        if not self.sesion:
            raise RuntimeError("Debe iniciar sesión primero")
        return self.sesion

    # --- Flujos de alto nivel invocados por la Vista ---

    def login(self, usuario: str, clave: str) -> None:
        """Realiza login contra el servidor y guarda la sesión."""
        self.sesion = self.rpc.login(usuario, clave)
        rol = "admin" if self.sesion.es_admin else "operador"
        self.logger.log("info", "ControladorCliente", f"Login OK como {rol}")

    def subir_archivo(self, nombre: str) -> None:
        """Sube un archivo G-code al servidor."""
        ses = self._req_sesion()
        contenido = self.archivos.leer_binario(nombre)
        self.rpc.subir_archivo(ses, nombre, contenido)
        self.logger.log("info", "ControladorCliente", f"Archivo subido: {nombre}")

    def set_modo(self, manual: bool, absoluto: bool) -> None:
        """Selecciona modo manual/automático y absoluto/relativo."""
        ses = self._req_sesion()
        self.modos.set_modo(ses, manual, absoluto)
        self.logger.log(
            "info",
            "ControladorCliente",
            f"Modo actualizado: manual={manual}, absoluto={absoluto}",
        )

    def mover(self, x: float, y: float, z: float, v: Optional[float] = None) -> None:
        """Mueve el robot a la posición indicada."""
        ses = self._req_sesion()
        self.modos.mover(ses, x, y, z, v)
        self.logger.log("info", "ControladorCliente", f"mover({x}, {y}, {z}, v={v})")

    def home(self) -> None:
        """Envía el robot a la posición home."""
        ses = self._req_sesion()
        self.modos.home(ses)
        self.logger.log("info", "ControladorCliente", "home()")

    def gripper(self, on: bool) -> None:
        """Abre o cierra la pinza."""
        ses = self._req_sesion()
        self.modos.gripper(ses, on)
        self.logger.log("info", "ControladorCliente", f"gripper(on={on})")

    def ejecutar_archivo(self, nombre: str) -> None:
        """Ejecuta un archivo G-code almacenado en el servidor."""
        ses = self._req_sesion()
        self.modos.ejecutar_archivo(ses, nombre)
        self.logger.log("info", "ControladorCliente", f"ejecutar_archivo({nombre})")

    def reporte_operador(self) -> List[str]:
        """Obtiene el reporte de actividad del operador actual."""
        ses = self._req_sesion()
        return list(self.reportes.operador(ses))

    def reporte_admin(self, filtros: Dict[str, Any]) -> List[str]:
        """Obtiene el reporte de actividad global (admin)."""
        ses = self._req_sesion()
        return list(self.reportes.admin(ses, filtros))

    def reporte_log(self, filtros: Dict[str, Any]) -> List[str]:
        """Obtiene el reporte del log técnico (admin)."""
        ses = self._req_sesion()
        return list(self.reportes.log(ses, filtros))
