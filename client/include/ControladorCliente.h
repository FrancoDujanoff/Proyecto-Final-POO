
#ifndef CONTROLADORCLIENTE_H
#define CONTROLADORCLIENTE_H

#include cadena (string)
#include vector



/// 
/// class ControladorCliente

class ControladorCliente
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  ControladorCliente();

  /// 
  /// Empty Destructor
  virtual ~ControladorCliente();



  /// 
  void iniciar()
  {
  }


  /// 
  /// @return bool
  /// @param  nombre 
  /// @param  password 
  bool solicitarLogin(cadena (string) nombre, cadena (string) password)
  {
  }


  /// 
  /// @param  x 
  /// @param  y 
  /// @param  z 
  void solicitarMoverRobot(float x, float y, float z)
  {
  }


  /// 
  /// @param  rutaArchivo 
  void solicitarSubirArchivo(cadena (string) rutaArchivo)
  {
  }


  /// 
  void solicitarReporte()
  {
  }


  /// 
  /// @param  estado 
  void actualizarVistaConEstado(cadena (string) estado)
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


  ClienteRPC clienteRPC;
  VistaGUI vista;
  Usuario usuarioActual;

  // Public attribute accessor methods  



  /// 
  /// Set the value of clienteRPC
  /// @param value the new value of clienteRPC
  void setClienteRPC(ClienteRPC value)
  {
    clienteRPC = value;
  }

  /// 
  /// Get the value of clienteRPC
  /// @return the value of clienteRPC
  ClienteRPC getClienteRPC()
  {
    return clienteRPC;
  }

  /// 
  /// Set the value of vista
  /// @param value the new value of vista
  void setVista(VistaGUI value)
  {
    vista = value;
  }

  /// 
  /// Get the value of vista
  /// @return the value of vista
  VistaGUI getVista()
  {
    return vista;
  }

  /// 
  /// Set the value of usuarioActual
  /// @param value the new value of usuarioActual
  void setUsuarioActual(Usuario value)
  {
    usuarioActual = value;
  }

  /// 
  /// Get the value of usuarioActual
  /// @return the value of usuarioActual
  Usuario getUsuarioActual()
  {
    return usuarioActual;
  }

  void initAttributes();

};

#endif // CONTROLADORCLIENTE_H
