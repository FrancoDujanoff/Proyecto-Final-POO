
#ifndef BASEDEDATOS_H
#define BASEDEDATOS_H

#include cadena (string)
#include vector



/// 
/// class BaseDeDatos

class BaseDeDatos
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  BaseDeDatos();

  /// 
  /// Empty Destructor
  virtual ~BaseDeDatos();



  /// 
  /// @param  connectionString 
  void conectar(cadena (string) connectionString)
  {
  }


  /// 
  /// @param  nombre 
  /// @param  clave 
  void obtenerUsuario(cadena (string) nombre, cadena (string) clave)
  {
  }


  /// 
  /// @param  nombre 
  void esAdmin(cadena (string) nombre)
  {
  }


};

#endif // BASEDEDATOS_H
