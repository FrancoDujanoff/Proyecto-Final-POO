
#ifndef USUARIO_H
#define USUARIO_H

#include cadena (string)
#include vector



/// 
/// class Usuario

class Usuario
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  Usuario();

  /// 
  /// Empty Destructor
  virtual ~Usuario();



  /// 
  /// @return bool
  bool esAdmin()
  {
  }

private:
  // Private attributes  


  int id;
  cadena (string) name;
  cadena (string) tipoUsuario;

  // Public attribute accessor methods  



  /// 
  /// Set the value of id
  /// @param value the new value of id
  void setId(int value)
  {
    id = value;
  }

  /// 
  /// Get the value of id
  /// @return the value of id
  int getId()
  {
    return id;
  }

  /// 
  /// Set the value of name
  /// @param value the new value of name
  void setName(cadena (string) value)
  {
    name = value;
  }

  /// 
  /// Get the value of name
  /// @return the value of name
  cadena (string) getName()
  {
    return name;
  }

  /// 
  /// Set the value of tipoUsuario
  /// @param value the new value of tipoUsuario
  void setTipoUsuario(cadena (string) value)
  {
    tipoUsuario = value;
  }

  /// 
  /// Get the value of tipoUsuario
  /// @return the value of tipoUsuario
  cadena (string) getTipoUsuario()
  {
    return tipoUsuario;
  }

  void initAttributes();

};

#endif // USUARIO_H
