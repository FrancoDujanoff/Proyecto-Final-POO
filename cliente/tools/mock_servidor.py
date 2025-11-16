from xmlrpc.server import SimpleXMLRPCServer
from xmlrpc.server import SimpleXMLRPCRequestHandler
import datetime

# Constantes de configuración
HOST = 'localhost'
PORT = 8000

class ApiRobotMock:
    """
    Implementa la API simulada del robot.
    Maneja un estado simple (sesiones activas) para simular la autenticación.
    """
    def __init__(self):
        # Usamos un 'set' para almacenar los tokens de sesión activos
        self.sesiones_activas = set()
        print(f"Instancia de ApiRobotMock creada.")
        print("Usuarios válidos: {'admin': '1234'}")

    def _validar_token(self, token_sesion):
        """Método privado para validar un token."""
        if token_sesion not in self.sesiones_activas:
            # En XML-RPC, lanzar una excepción es la forma estándar
            # de comunicar un error al cliente.
            raise ValueError(f"Token de sesión no válido o expirado: {token_sesion}")
        return True

    # --- Implementación de la API Pública ---

    def conectar(self, usuario, clave):
        """
        Intenta autenticar a un usuario. Si tiene éxito, devuelve un token.
        """
        print(f"Intento de conexión recibido: usuario={usuario}")
        if usuario == 'admin' and clave == '1234':
            # Generamos un token simple basado en la hora
            token = f"sesion_token_{datetime.datetime.now().timestamp()}"
            self.sesiones_activas.add(token)
            print(f"Conexión exitosa. Token generado: {token}")
            return token
        else:
            print("Conexión fallida: Credenciales incorrectas.")
            raise ValueError("Usuario o clave incorrectos")

    def desconectar(self, token_sesion):
        """
        Cierra una sesión de cliente.
        """
        print(f"Intento de desconexión recibido: token={token_sesion}")
        # .discard() no lanza error si el token no existe, lo cual es seguro.
        self.sesiones_activas.discard(token_sesion)
        print("Sesión cerrada.")
        return True

    def enviar_comando_gcode(self, token_sesion, comando_gcode):
        """
        Recibe un comando G-Code, lo valida y simula una respuesta 'OK'.
        """
        self._validar_token(token_sesion) # Primero, validar sesión
        
        print(f"Comando G-Code RECIBIDO: '{comando_gcode}'")
        
        # Simulación de validación de G-Code
        if not comando_gcode.strip().startswith(('G', 'M')):
            print("-> ERROR: Comando no válido.")
            return "ERROR: Comando G-Code no válido"
        
        print("-> OK")
        return "OK" # Respuesta simulada del robot

    def obtener_estado_robot(self, token_sesion):
        """
        Devuelve un diccionario (struct) con el estado simulado del robot.
        """
        self._validar_token(token_sesion)
        print("Solicitud de estado recibida.")
        
        # Datos simulados
        estado = {
            "estado_robot": "listo",
            "posicion_actual": [10.5, 20.0, 5.1],
            "comando_en_ejecucion": "None"
        }
        return estado

    def obtener_historial_logs(self, token_sesion, lineas_maximas):
        """
        Devuelve una lista (array) de strings con logs simulados.
        """
        self._validar_token(token_sesion)
        print(f"Solicitud de logs recibida (max: {lineas_maximas} líneas).")
        
        # Logs simulados
        logs = [
            "[INFO] Servidor iniciado.",
            "[DEBUG] Conexión de robot establecida en COM3.",
            "[INFO] Sesión 'sesion_token_...' iniciada.",
            f"[CMD] G01 X10.5 Y20.0 Z5.1",
            "[WARNING] Límite de carrera Z casi alcanzado."
        ]
        # Devolvemos las primeras 'lineas_maximas' para mantener orden cronológico
        # (los tests esperan que la línea 0 sea '[INFO] Servidor iniciado.')
        return logs[:lineas_maximas]

# --- Configuración y arranque del servidor ---

class RequestHandler(SimpleXMLRPCRequestHandler):
    """Limita las peticiones a la ruta /RPC2"""
    rpc_paths = ('/RPC2',)

def iniciar_servidor():
    """
    Configura e inicia el servidor XML-RPC.
    """
    print(f"Iniciando Mock Server en http://{HOST}:{PORT}/RPC2 ...")
    
    with SimpleXMLRPCServer((HOST, PORT),
                            requestHandler=RequestHandler,
                            allow_none=True) as server:
        
        server.register_introspection_functions()
        
        # Registramos una *instancia* de nuestra clase.
        # El servidor expondrá todos los métodos públicos de esta instancia.
        server.register_instance(ApiRobotMock())
        
        print("Servidor listo. Esperando peticiones (Ctrl+C para salir).")
        
        try:
            # Iniciamos el bucle principal del servidor
            server.serve_forever()
        except KeyboardInterrupt:
            print("\nServidor detenido.")

if __name__ == "__main__":
    iniciar_servidor()