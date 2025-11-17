import os

class GestorArchivosLocal:

    """
    Clase auxiliar para manejar archivos en la computadora del CLIENTE.
    """
    
    @staticmethod
    def leer_archivo(ruta_completa):
        if not os.path.exists(ruta_completa):
            return None, "Error: El archivo no existe."
        
        try:
            with open(ruta_completa, 'r') as f:
                contenido = f.read()
            
            # Extraemos solo el nombre (ej. C:/user/docs/test.gcode -> test.gcode)
            nombre_base = os.path.basename(ruta_completa)
            return (nombre_base, contenido), "OK"
            
        except Exception as e:
            return None, f"Error al leer archivo: {str(e)}"