import xmlrpc.client

class ClienteRPC:

    """
    Clase Proxy que encapsula la comunicación con el Servidor XML-RPC.
    Sigue el patrón de diseño 'Proxy', actuando como representante local del robot remoto.
    """

    def __init__(self, url="http://localhost:8080"):

        # xmlrpc.client es la librería estándar de Python para este protocolo

        self.proxy = xmlrpc.client.ServerProxy(url)
        self.usuario = ""
        self.clave = ""
        self.conectado = False

    def set_credenciales(self, usuario, clave):

        """Guarda las credenciales para usarlas en cada petición."""

        self.usuario = usuario
        self.clave = clave

    def _llamar_protegido(self, funcion_rpc, *args):

        """
        Método helper (privado) para realizar llamadas seguras.
        Inyecta automáticamente usuario y clave, y maneja excepciones de red.
        """

        if not self.usuario:
            return "Error: Credenciales no configuradas."
        
        try:
            
            return funcion_rpc(self.usuario, self.clave, *args)
        except xmlrpc.client.Fault as err:

            # Errores de lógica del servidor (ej. "Acceso denegado", "Robot desconectado")
            return f"Error del Servidor (Fault {err.faultCode}): {err.faultString}"
        except ConnectionRefusedError:

            return "Error: No se pudo conectar. ¿El servidor C++ está encendido?"
        except Exception as e:

            return f"Error de Red: {str(e)}"

    #Metodos Publicos

    def intentar_login(self):

        """Prueba si las credenciales actuales son válidas."""
        
        try:
            es_valido = self.proxy.robot.login(self.usuario, self.clave)
            self.conectado = es_valido
            if es_valido:
                return "Login Exitoso"
            else:
                return "Error: Usuario o clave incorrectos"
        except Exception as e:
            self.conectado = False
            return f"Error de conexión al intentar login: {str(e)}"

    def conectar_hardware(self, estado: bool):
        return self._llamar_protegido(self.proxy.robot.controlarConexion, estado)

    def controlar_motores(self, estado: bool):
        return self._llamar_protegido(self.proxy.robot.controlarMotores, estado)
    
    def ir_a_home(self):
        return self._llamar_protegido(self.proxy.robot.irAHome)

    def mover_lineal(self, x, y, z, vel=10.0):
        return self._llamar_protegido(self.proxy.robot.mover, float(x), float(y), float(z), float(vel))

    def controlar_efector(self, estado: bool):
        return self._llamar_protegido(self.proxy.robot.controlarEfector, estado)

    def obtener_reporte(self):
        return self._llamar_protegido(self.proxy.robot.solicitarReporteEstado)

    #Gestion de Archivos

    def listar_archivos_servidor(self):
        return self._llamar_protegido(self.proxy.robot.listarArchivos)

    def subir_archivo_gcode(self, nombre_archivo, contenido):
        return self._llamar_protegido(self.proxy.robot.subirArchivo, nombre_archivo, contenido)

    def ejecutar_archivo_remoto(self, nombre_archivo):
        return self._llamar_protegido(self.proxy.robot.ejecutarArchivo, nombre_archivo)

    #Aprendizaje

    def iniciar_aprendizaje(self):
        return self._llamar_protegido(self.proxy.robot.iniciarAprendizaje)

    def registrar_punto(self):
        return self._llamar_protegido(self.proxy.robot.registrarPunto)
    
    def guardar_trayectoria(self, nombre_archivo):
        return self._llamar_protegido(self.proxy.robot.guardarTrayectoria, nombre_archivo)
    
    def pausar_robot(self, segundos):
        return self._llamar_protegido(self.proxy.robot.pausar, float(segundos))

    def set_modo_coordenadas(self, absoluto: bool):
        return self._llamar_protegido(self.proxy.robot.setModoCoordenadas, absoluto)

    def definir_posicion_actual(self, x, y, z):
        return self._llamar_protegido(self.proxy.robot.definirPosicionActual, float(x), float(y), float(z), 0.0)

    def controlar_ventilador(self, estado: bool):
        return self._llamar_protegido(self.proxy.robot.controlarVentilador, estado)

    def reporte_endstops(self):
        return self._llamar_protegido(self.proxy.robot.solicitarReporteEndstops)