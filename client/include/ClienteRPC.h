
#ifndef CLIENTERPC_H
#define CLIENTERPC_H

#include cadena (string)


/// 
/// class ClienteRPC

class ClienteRPC
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  ClienteRPC();

  /// 
  /// Empty Destructor
  virtual ~ClienteRPC();



  /// 
  /// @return bool
  /// @param  host 
  /// @param  puerto 
  bool conectar(cadena (string) host, int puerto)
  {
  }


  /// 
  /// @return Usuario
  /// @param  name 
  /// @param  password 
  Usuario login(cadena (string) name, cadena (string) password)
  {
  }


  /// 
  /// @return bool
  /// @param  x 
  /// @param  y 
  /// @param  z 
  bool mover(float x, float y, float z)
  {
  }


  /// 
  /// @return bool
  /// @param  rutaLocal 
  bool subirArchivo(cadena (string) rutaLocal)
  {
  }


  /// 
  /// @return cadena (string)
  cadena (string) obtenerReporte()
  {
  }

private:
  // Private attributes  


  cadena (string) host;
  int puert;

  // Public attribute accessor methods  



  /// 
  /// Set the value of host
  /// @param value the new value of host
  void setHost(cadena (string) value)
  {
    host = value;
  }

  /// 
  /// Get the value of host
  /// @return the value of host
  cadena (string) getHost()
  {
    return host;
  }

  /// 
  /// Set the value of puert
  /// @param value the new value of puert
  void setPuert(int value)
  {
    puert = value;
  }

  /// 
  /// Get the value of puert
  /// @return the value of puert
  int getPuert()
  {
    return puert;
  }

  void initAttributes();

};

#endif // CLIENTERPC_H
