
#ifndef CONTROLADORGENERAL_H
#define CONTROLADORGENERAL_H

#include cadena (string)
#include vector



/// 
/// class ControladorGeneral

class ControladorGeneral
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  ControladorGeneral();

  /// 
  /// Empty Destructor
  virtual ~ControladorGeneral();



  /// 
  /// @return Usuario
  /// @param  usuario 
  /// @param  password 
  Usuario login(cadena (string) usuario, cadena (string) password)
  {
  }


  /// 
  /// @param  usuario 
  /// @param  x 
  /// @param  y 
  /// @param  z 
  void moverRobot(Usuario usuario, float x, float y, float z)
  {
  }


  /// 
  /// @param  usuario 
  /// @param  nombre 
  /// @param  contenido 
  void subirArchivoGCode(Usuario usuario, cadena (string) nombre, cadena (string) contenido)
  {
  }


  /// 
  void enviarMensaje()
  {
  }


  /// 
  void recibirMensaje()
  {
  }


  /// 
  void leerMensaje()
  {
  }

private:
  // Private attributes  


  ControladorRobot controladorRobot;
  GestorUsuarios gestorUsuarios;
  GestorDeArchivos gestorArchivos;
  GestorDeReportes gestorReportes;
  PALogger logger;

  // Public attribute accessor methods  



  /// 
  /// Set the value of controladorRobot
  /// @param value the new value of controladorRobot
  void setControladorRobot(ControladorRobot value)
  {
    controladorRobot = value;
  }

  /// 
  /// Get the value of controladorRobot
  /// @return the value of controladorRobot
  ControladorRobot getControladorRobot()
  {
    return controladorRobot;
  }

  /// 
  /// Set the value of gestorUsuarios
  /// @param value the new value of gestorUsuarios
  void setGestorUsuarios(GestorUsuarios value)
  {
    gestorUsuarios = value;
  }

  /// 
  /// Get the value of gestorUsuarios
  /// @return the value of gestorUsuarios
  GestorUsuarios getGestorUsuarios()
  {
    return gestorUsuarios;
  }

  /// 
  /// Set the value of gestorArchivos
  /// @param value the new value of gestorArchivos
  void setGestorArchivos(GestorDeArchivos value)
  {
    gestorArchivos = value;
  }

  /// 
  /// Get the value of gestorArchivos
  /// @return the value of gestorArchivos
  GestorDeArchivos getGestorArchivos()
  {
    return gestorArchivos;
  }

  /// 
  /// Set the value of gestorReportes
  /// @param value the new value of gestorReportes
  void setGestorReportes(GestorDeReportes value)
  {
    gestorReportes = value;
  }

  /// 
  /// Get the value of gestorReportes
  /// @return the value of gestorReportes
  GestorDeReportes getGestorReportes()
  {
    return gestorReportes;
  }

  /// 
  /// Set the value of logger
  /// @param value the new value of logger
  void setLogger(PALogger value)
  {
    logger = value;
  }

  /// 
  /// Get the value of logger
  /// @return the value of logger
  PALogger getLogger()
  {
    return logger;
  }

  void initAttributes();

};

#endif // CONTROLADORGENERAL_H
