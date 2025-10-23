
#ifndef MENSAJECLIENTE_H
#define MENSAJECLIENTE_H

#include cadena (string)


/// 
/// class MensajeCliente

class MensajeCliente
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  MensajeCliente();

  /// 
  /// Empty Destructor
  virtual ~MensajeCliente();



  /// 
  void setCodigo()
  {
  }


  /// 
  void getCodigo()
  {
  }


  /// 
  void setDato()
  {
  }


  /// 
  void getDato()
  {
  }

private:
  // Private attributes  


  cadena (string) codigo;
  cadena (string) mensaje;

  // Public attribute accessor methods  



  /// 
  /// Set the value of codigo
  /// @param value the new value of codigo
  void setCodigo(cadena (string) value)
  {
    codigo = value;
  }

  /// 
  /// Get the value of codigo
  /// @return the value of codigo
  cadena (string) getCodigo()
  {
    return codigo;
  }

  /// 
  /// Set the value of mensaje
  /// @param value the new value of mensaje
  void setMensaje(cadena (string) value)
  {
    mensaje = value;
  }

  /// 
  /// Get the value of mensaje
  /// @return the value of mensaje
  cadena (string) getMensaje()
  {
    return mensaje;
  }

  void initAttributes();

};

#endif // MENSAJECLIENTE_H
