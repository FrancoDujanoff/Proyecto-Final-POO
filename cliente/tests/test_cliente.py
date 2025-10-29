import pytest
import threading
import time
from xmlrpc.server import SimpleXMLRPCServer

# Importamos las clases de nuestros otros archivos
from cliente.tools.mock_servidor import ApiRobotMock, RequestHandler, HOST, PORT
from cliente.src.cliente_robot import ClienteRobot

@pytest.fixture(scope="module")
def mock_server_en_hilo():
    """
    Fixture de Pytest: Inicia el Mock Server en un hilo separado
    antes de que comiencen las pruebas de este módulo, y lo detiene después.
    """
    print("\nIniciando Mock Server en hilo (Fixture)...")
    
    # Configuramos el servidor
    server = SimpleXMLRPCServer((HOST, PORT),
                                requestHandler=RequestHandler,
                                allow_none=True,
                                logRequests=False) # Silenciamos logs de peticiones
    
    server.register_introspection_functions()
    server.register_instance(ApiRobotMock())
    
    # Iniciamos el servidor en un hilo (demonio)
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True # El hilo morirá si el test principal muere
    server_thread.start()
    
    print(f"Servidor iniciado en {HOST}:{PORT}.")
    time.sleep(0.1) # Damos un instante para que el hilo inicie

    # 'yield' es el punto donde se ejecutan las pruebas
    yield f"http://{HOST}:{PORT}/RPC2"
    
    # --- Teardown (limpieza) ---
    print("\nDeteniendo Mock Server (Fixture)...")
    server.shutdown() # Detenemos el servidor
    server.server_close() # Cerramos el socket
    server_thread.join(timeout=1.0) # Esperamos que el hilo termine
    print("Servidor detenido.")

@pytest.fixture
def cliente():
    """
    Fixture que provee una instancia limpia de ClienteRobot para cada test.
    """
    return ClienteRobot(HOST, PORT)

# --- INICIO DE CASOS DE PRUEBA ---

def test_conexion_exitosa(mock_server_en_hilo, cliente):
    """
    Verifica que la conexión con credenciales correctas funciona
    y almacena el token.
    """
    # El fixture 'mock_server_en_hilo' asegura que el servidor está corriendo
    assert cliente.token_sesion is None
    resultado = cliente.conectar("admin", "1234")
    
    assert resultado is True
    assert cliente.token_sesion is not None
    assert "sesion_token_" in cliente.token_sesion

def test_conexion_fallida_clave(mock_server_en_hilo, cliente):
    """
    Verifica que el servidor rechaza credenciales incorrectas.
    """
    resultado = cliente.conectar("admin", "clave_mala")
    
    assert resultado is False
    assert cliente.token_sesion is None

def test_operacion_sin_conexion(mock_server_en_hilo, cliente):
    """
    Verifica que el cliente lanza una excepción local si se
    intenta operar sin un token.
    """
    # 'pytest.raises' verifica que el bloque de código lanza la excepción esperada
    with pytest.raises(PermissionError) as e:
        cliente.enviar_gcode("G01 X100")
    
    # Verificamos que el mensaje de error sea el correcto
    assert "No está conectado" in str(e.value)

def test_operaciones_basicas_tras_conexion(mock_server_en_hilo, cliente):
    """
    Prueba de integración: Conectar, operar y desconectar.
    """
    # 1. Conectar
    cliente.conectar("admin", "1234")
    assert cliente.token_sesion is not None

    # 2. Enviar G-Code
    resp = cliente.enviar_gcode("G01 X100")
    assert resp == "OK"

    # 3. Obtener Estado
    estado = cliente.get_estado()
    assert estado is not None
    assert estado["estado_robot"] == "listo"
    assert estado["posicion_actual"] == [10.5, 20.0, 5.1]
    
    # 4. Obtener Logs
    logs = cliente.get_logs(lineas=3)
    assert len(logs) == 3
    assert "[INFO] Servidor iniciado." in logs[0]
    
    # 5. Desconectar
    cliente.desconectar()
    assert cliente.token_sesion is None

def test_token_invalido_en_servidor(mock_server_en_hilo, cliente):
    """
    Simula un token corrupto para verificar que el servidor lo rechaza.
    """
    cliente.conectar("admin", "1234")
    # Corrompemos el token localmente
    cliente.token_sesion = "token_falso_123" 
    
    # El servidor (Mock) debe detectar el token falso y devolver un error
    # que el cliente (proxy) convierte en un Fault.
    # Nuestro cliente lo captura e imprime un error.
    
    # Probamos get_estado
    assert cliente.get_estado() is None
    
    # Probamos enviar_gcode
    resp = cliente.enviar_gcode("G01")
    assert "ERROR" in resp
    assert "Token de sesión no válido" in resp