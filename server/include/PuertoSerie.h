
#ifndef PUERTOSERIE_H
#define PUERTOSERIE_H

#include cadena (string)


/// 
/// class PuertoSerie

class PuertoSerie
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  PuertoSerie();

  /// 
  /// Empty Destructor
  virtual ~PuertoSerie();



  /// 
  void abrir()
  {
  }


  /// 
  void cerrar()
  {
  }


  /// 
  /// @param  comando 
  void escribir(cadena (string) comando)
  {
  }


  /// 
  /// @return cadena (string)
  cadena (string) leerDatos()
  {
  }

private:
  // Private attributes  


  cadena (string) puerto;
  int baudrate;
  bool estado;

  // Public attribute accessor methods  



  /// 
  /// Set the value of puerto
  /// @param value the new value of puerto
  void setPuerto(cadena (string) value)
  {
    puerto = value;
  }

  /// 
  /// Get the value of puerto
  /// @return the value of puerto
  cadena (string) getPuerto()
  {
    return puerto;
  }

  /// 
  /// Set the value of baudrate
  /// @param value the new value of baudrate
  void setBaudrate(int value)
  {
    baudrate = value;
  }

  /// 
  /// Get the value of baudrate
  /// @return the value of baudrate
  int getBaudrate()
  {
    return baudrate;
  }

  /// 
  /// Set the value of estado
  /// @param value the new value of estado
  void setEstado(bool value)
  {
    estado = value;
  }

  /// 
  /// Get the value of estado
  /// @return the value of estado
  bool getEstado()
  {
    return estado;
  }

  void initAttributes();

};

#endif // PUERTOSERIE_H
