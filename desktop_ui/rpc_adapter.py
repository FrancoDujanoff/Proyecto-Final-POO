"""
Adaptador simple XML-RPC para la UI de escritorio.
Encapsula xmlrpc.client.ServerProxy, mantiene token de sesión (mecanismo actual).

Usa los nombres de método del Mock Server por defecto:
 - conectar(usuario, clave) -> devuelve token
 - desconectar(token)
 - enviar_comando_gcode(token, comando)
 - obtener_estado_robot(token)
 - obtener_historial_logs(token, lineas)

Si quieres apuntar al servidor C++ adapta `host`/`port` o los nombres de método.
"""
from __future__ import annotations
import xmlrpc.client
from typing import Optional, Any
from pathlib import Path


class RPCAdapter:
    def __init__(self, host: str = "localhost", port: int = 8000, allow_none: bool = True):
        self.host = host
        self.port = int(port)
        self.allow_none = allow_none
        self.url = f"http://{self.host}:{self.port}/RPC2"
        self.proxy = xmlrpc.client.ServerProxy(self.url, allow_none=self.allow_none)
        self.token: Optional[str] = None
        self.contrato: str = "mock"  # "mock" o "cpp" (auto-detectado al conectar)

    def set_server(self, host: str, port: int):
        self.host = host
        self.port = int(port)
        self.url = f"http://{self.host}:{self.port}/RPC2"
        self.proxy = xmlrpc.client.ServerProxy(self.url, allow_none=self.allow_none)
        self.contrato = "mock"  # Reset cuando cambia servidor

    def _detectar_contrato(self, usuario: str, clave: str) -> str:
        """Intenta ambos contratos y devuelve cuál funciona: 'mock' o 'cpp'."""
        # Intenta primero contrato mock
        try:
            token = self.proxy.conectar(usuario, clave)
            return "mock"
        except (xmlrpc.client.Fault, Exception):
            pass
        # Intenta contrato C++
        try:
            token = self.proxy.robot.login(usuario, clave)
            return "cpp"
        except (xmlrpc.client.Fault, Exception):
            pass
        raise ConnectionError("No se pudo conectar: servidor no responde a ningún contrato conocido.")

    def conectar(self, usuario: str, clave: str) -> bool:
        try:
            # Auto-detectar contrato
            self.contrato = self._detectar_contrato(usuario, clave)
            
            if self.contrato == "mock":
                token = self.proxy.conectar(usuario, clave)
            else:  # cpp
                token = self.proxy.robot.login(usuario, clave)
            
            self.token = token
            return True
        except xmlrpc.client.Fault as e:
            raise RuntimeError(f"Error API: {e.faultString}")
        except ConnectionRefusedError as e:
            raise ConnectionError("No se pudo conectar al servidor.")
        except Exception as e:
            raise

    def desconectar(self) -> bool:
        if not self.token:
            return False
        try:
            self.proxy.desconectar(self.token)
        except Exception:
            # Ignorar errores de desconexión pero limpiar token
            pass
        self.token = None
        return True

    def enviar_gcode(self, comando: str) -> Any:
        if not self.token:
            raise PermissionError("No conectado")
        try:
            if self.contrato == "mock":
                return self.proxy.enviar_comando_gcode(self.token, comando)
            else:  # cpp
                return self.proxy.robot.enviar_gcode(self.token, comando)
        except xmlrpc.client.Fault as e:
            return f"ERROR: {e.faultString}"

    def obtener_estado(self) -> Any:
        if not self.token:
            raise PermissionError("No conectado")
        try:
            if self.contrato == "mock":
                return self.proxy.obtener_estado_robot(self.token)
            else:  # cpp
                return self.proxy.robot.obtener_estado(self.token)
        except xmlrpc.client.Fault as e:
            return None

    def obtener_logs(self, lineas: int = 10) -> Any:
        if not self.token:
            raise PermissionError("No conectado")
        try:
            if self.contrato == "mock":
                return list(self.proxy.obtener_historial_logs(self.token, int(lineas)))
            else:  # cpp
                return list(self.proxy.robot.obtener_logs(self.token, int(lineas)))
        except xmlrpc.client.Fault as e:
            return []
