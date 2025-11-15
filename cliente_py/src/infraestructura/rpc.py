"""Adaptador RPC. Encapsula la comunicación con el servidor."""

import xmlrpc.client
from typing import Any, Dict, List, Optional

from ..config import RPCConfig
from .seguridad import Sesion, AutorizacionDenegada


class ClienteRPC:
    """Encapsula llamadas XML-RPC al servidor."""

    def __init__(self, cfg: Optional[RPCConfig] = None) -> None:
        self.cfg = cfg or RPCConfig()
        # Proxy XML-RPC. Ajustar la URL en config.py si hace falta.
        self._proxy = xmlrpc.client.ServerProxy(self.cfg.url, allow_none=True)

    # --- Autenticación ---

    def login(self, usuario: str, clave: str) -> Sesion:
        """Autentica con el servidor y devuelve una sesión."""
        # OJO: después hay que ajustar este nombre y contrato
        # a lo que efectivamente expone el servidor C++.
        resp = self._proxy.login(usuario, clave)
        if not resp or not resp.get("ok"):
            raise AutorizacionDenegada("Credenciales inválidas")
        return Sesion(
            usuario=usuario,
            token=resp.get("token"),
            es_admin=bool(resp.get("admin")),
        )

    # --- Servicios administrativos / operativos ---
    # Estos nombres hay que alinearlos luego con el servidor.

    def conectar_robot(self, ses: Sesion) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.conectar_robot(ses.usuario, ses.token)

    def desconectar_robot(self, ses: Sesion) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.desconectar_robot(ses.usuario, ses.token)

    def habilitar_remotos(self, ses: Sesion, onoff: bool) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.habilitar_remotos(ses.usuario, ses.token, bool(onoff))

    def activar_motores(self, ses: Sesion, onoff: bool) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.activar_motores(ses.usuario, ses.token, bool(onoff))

    def listar_comandos(self, ses: Sesion) -> List[str]:
        ses.assert_activa()
        return list(self._proxy.listar_comandos(ses.usuario, ses.token))

    # --- Reportes ---

    def reporte_operador(self, ses: Sesion) -> List[str]:
        ses.assert_activa()
        return list(self._proxy.reporte_operador(ses.usuario, ses.token))

    def reporte_admin(self, ses: Sesion, filtros: Optional[Dict[str, Any]] = None) -> List[str]:
        ses.assert_activa()
        return list(self._proxy.reporte_admin(ses.usuario, ses.token, filtros or {}))

    def reporte_log(self, ses: Sesion, filtros: Optional[Dict[str, Any]] = None) -> List[str]:
        ses.assert_activa()
        return list(self._proxy.reporte_log(ses.usuario, ses.token, filtros or {}))

    # --- Archivos (G-code) ---

    def subir_archivo(self, ses: Sesion, nombre: str, contenido: bytes) -> Dict[str, Any]:
        ses.assert_activa()
        b64 = xmlrpc.client.Binary(contenido)
        return self._proxy.subir_archivo(ses.usuario, ses.token, nombre, b64)

    # --- Modos y movimiento ---

    def seleccionar_modo(self, ses: Sesion, manual: bool, absoluto: bool) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.seleccionar_modo(ses.usuario, ses.token, manual, absoluto)

    def home(self, ses: Sesion) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.home(ses.usuario, ses.token)

    def mover(
        self,
        ses: Sesion,
        x: float,
        y: float,
        z: float,
        v: Optional[float] = None,
    ) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.mover(ses.usuario, ses.token, float(x), float(y), float(z), v)

    def gripper(self, ses: Sesion, onoff: bool) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.gripper(ses.usuario, ses.token, bool(onoff))

    def ejecutar_archivo(self, ses: Sesion, nombre: str) -> Dict[str, Any]:
        ses.assert_activa()
        return self._proxy.ejecutar_archivo(ses.usuario, ses.token, nombre)
