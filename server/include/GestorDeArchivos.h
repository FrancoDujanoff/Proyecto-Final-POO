
#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include cadena (string)


/// 
/// class GestorDeArchivos

class GestorDeArchivos
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  GestorDeArchivos();

  /// 
  /// Empty Destructor
  virtual ~GestorDeArchivos();



  /// 
  /// @param  usuario 
  /// @param  nombreArchivo 
  /// @param  contenido 
  void almacenarArchivo(Usuario usuario, cadena (string) nombreArchivo, cadena (string) contenido)
  {
  }


  /// 
  /// @param  usuario 
  /// @param  nombreArchivo 
  void validarAccesoArchivo(Usuario usuario, cadena (string) nombreArchivo)
  {
  }


  /// 
  /// @param  usuario 
  /// @param  nombreArchivo 
  void obtenerContenidoArchivo(Usuario usuario, cadena (string) nombreArchivo)
  {
  }

private:
  // Private attributes  


  cadena (string) directorioBase;

  // Public attribute accessor methods  



  /// 
  /// Set the value of directorioBase
  /// @param value the new value of directorioBase
  void setDirectorioBase(cadena (string) value)
  {
    directorioBase = value;
  }

  /// 
  /// Get the value of directorioBase
  /// @return the value of directorioBase
  cadena (string) getDirectorioBase()
  {
    return directorioBase;
  }

  void initAttributes();

};

#endif // GESTORDEARCHIVOS_H
