import xmlrpc.client
import sys

class ClienteRobot:
    """
    Clase cliente para interactuar con el Servidor del Robot vía XML-RPC.
    
    Encapsula la lógica de conexión, autenticación (manejo de token)
    y llamadas a la API remota.
    """
    def __init__(self, host='localhost', puerto=8000):
        url_servidor = f"http://{host}:{puerto}/RPC2"
        # Creamos el 'proxy' que representa al servidor.
        # Es nuestro "control remoto".
        self.proxy = xmlrpc.client.ServerProxy(url_servidor)
        self.token_sesion = None # Aún no estamos autenticados
        print(f"Cliente inicializado. Apuntando a: {url_servidor}")

    def conectar(self, usuario, clave):
        """
        Se conecta al servidor y obtiene un token de sesión.
        """
        try:
            print(f"Intentando conectar como '{usuario}'...")
            self.token_sesion = self.proxy.conectar(usuario, clave)
            print(f"Conexión exitosa. Token recibido: {self.token_sesion}")
            return True
        except xmlrpc.client.Fault as err:
            # 'Fault' es la excepción estándar de XML-RPC para errores de API
            print(f"Error de conexión (API): {err.faultString}")
            self.token_sesion = None
            return False
        except ConnectionRefusedError as err:
            print(f"Error de red: {err}. ¿Está el servidor encendido?")
            self.token_sesion = None
            return False

    def desconectar(self):
        """
        Informa al servidor que la sesión se cierra.
        """
        if not self.token_sesion:
            print("Ya estaba desconectado.")
            return

        print("Desconectando del servidor...")
        try:
            self.proxy.desconectar(self.token_sesion)
            print("Desconexión exitosa.")
        except xmlrpc.client.Fault as err:
            print(f"Error al desconectar: {err.faultString}")
        finally:
            # Independientemente de la respuesta, borramos el token local
            self.token_sesion = None

    def _verificar_conexion(self):
        """Método helper para asegurar que tenemos un token."""
        if not self.token_sesion:
            # Lanzamos una excepción local
            raise PermissionError("No está conectado. Llame a 'conectar()' primero.")

    def enviar_gcode(self, comando):
        """
        Envía un comando G-Code al robot (requiere conexión).
        """
        self._verificar_conexion() # Asegura que estamos conectados
        try:
            print(f"Enviando G-Code: '{comando}' ...")
            respuesta = self.proxy.enviar_comando_gcode(self.token_sesion, comando)
            print(f"Respuesta del servidor: {respuesta}")
            return respuesta
        except xmlrpc.client.Fault as err:
            print(f"Error de API al enviar comando: {err.faultString}")
            return f"ERROR: {err.faultString}"

    def get_estado(self):
        """
        Obtiene el estado actual del robot (requiere conexión).
        """
        self._verificar_conexion()
        try:
            print("Solicitando estado del robot...")
            estado = self.proxy.obtener_estado_robot(self.token_sesion)
            return estado
        except xmlrpc.client.Fault as err:
            print(f"Error de API al obtener estado: {err.faultString}")
            return None

    def get_logs(self, lineas=10):
        """
        Obtiene las últimas N líneas de log (requiere conexión).
        """
        self._verificar_conexion()
        try:
            print(f"Solicitando {lineas} líneas de log...")
            logs = self.proxy.obtener_historial_logs(self.token_sesion, lineas)
            return logs
        except xmlrpc.client.Fault as err:
            print(f"Error de API al obtener logs: {err.faultString}")
            return []

# --- Bloque de Demostración ---
# Este bloque solo se ejecuta si corremos este archivo directamente
# (Usando el perfil "Lanzar Cliente (Main)" de VSC)
if __name__ == "__main__":
    
    print("--- INICIO DE DEMO CLIENTE ROBOT ---")
    
    # 1. Creamos la instancia del cliente
    cliente = ClienteRobot()

    # 2. Intentamos una acción sin conectarnos (debe fallar)
    print("\n--- Prueba 1: Acción sin conexión (debe fallar) ---")
    try:
        cliente.enviar_gcode("G01 X100")
    except PermissionError as e:
        print(f"Error capturado (esperado): {e}")

    # 3. Conexión fallida (clave incorrecta)
    print("\n--- Prueba 2: Conexión fallida (clave incorrecta) ---")
    cliente.conectar("admin", "clave_mala")

    # 4. Conexión exitosa
    print("\n--- Prueba 3: Conexión exitosa ---")
    if not cliente.conectar("admin", "1234"):
        print("No se pudo conectar. Abortando demo.")
        sys.exit(1) # Salimos del script si la conexión falla

    # 5. Operaciones (ahora deben funcionar)
    print("\n--- Prueba 4: Operaciones ---")
    cliente.enviar_gcode("G01 X100 Y50")
    cliente.enviar_gcode("M03 S1000")
    cliente.enviar_gcode("Comando inválido") # Prueba de error de API

    # 6. Obtener estado
    print("\n--- Prueba 5: Obtener Estado ---")
    estado_robot = cliente.get_estado()
    if estado_robot:
        print(f"Estado recibido: {estado_robot['estado_robot']}")
        print(f"Posición actual: {estado_robot['posicion_actual']}")

    # 7. Obtener logs
    print("\n--- Prueba 6: Obtener Logs ---")
    logs = cliente.get_logs(lineas=3)
    if logs:
        print("Logs recibidos:")
        for linea in logs:
            print(f"  > {linea}")
            
    # 8. Desconectar
    print("\n--- Prueba 7: Desconexión ---")
    cliente.desconectar()
    cliente.desconectar() # Intentar desconectar de nuevo (es seguro)

    print("\n--- FIN DE DEMO CLIENTE ROBOT ---")