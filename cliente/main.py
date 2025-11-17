from ClienteRPC import ClienteRPC
from VistaGUI import VistaGUI

def main():
    
    # 1. Configuración
    # Si el servidor está en otra PC, cambie 'localhost' por la IP del servidor
    URL_SERVIDOR = "http://localhost:8080"

    # 2. Instanciar el Modelo (Proxy)
    cliente = ClienteRPC(URL_SERVIDOR)
    
    # 3. Instanciar la Vista (GUI) e inyectar el Modelo
    app = VistaGUI(cliente)
    
    # 4. Iniciar el bucle de eventos de la interfaz
    app.mainloop()

if __name__ == "__main__":
    main()