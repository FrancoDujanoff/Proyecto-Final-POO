
#ifndef VISTACONSOLA_H
#define VISTACONSOLA_H

#include cadena (string)
#include vector



/// 
/// class VistaConsola

class VistaConsola
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  VistaConsola();

  /// 
  /// Empty Destructor
  virtual ~VistaConsola();



  /// 
  void mostrarMenuPrincipal()
  {
  }


  /// 
  void solicitarCredenciales()
  {
  }


  /// 
  void solicitarCoordenadas()
  {
  }


  /// 
  void mostrarMensaje()
  {
  }


  /// 
  void mostrarReporte()
  {
  }

private:
  // Private attributes  


  ControladorCliente controlador;

  // Public attribute accessor methods  



  /// 
  /// Set the value of controlador
  /// @param value the new value of controlador
  void setControlador(ControladorCliente value)
  {
    controlador = value;
  }

  /// 
  /// Get the value of controlador
  /// @return the value of controlador
  ControladorCliente getControlador()
  {
    return controlador;
  }

  void initAttributes();

};

#endif // VISTACONSOLA_H
