
#ifndef GESTORDEREPORTES_H
#define GESTORDEREPORTES_H

#include cadena (string)
#include vector



/// 
/// class GestorDeReportes

class GestorDeReportes
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  GestorDeReportes();

  /// 
  /// Empty Destructor
  virtual ~GestorDeReportes();



  /// 
  /// @param  usuario 
  void generarReporteDeActividad(Usuario usuario)
  {
  }


  /// 
  void generarReporteAdmin()
  {
  }


  /// 
  void generarReporteDeLog()
  {
  }

private:
  // Private attributes  


  cadena (string) referenciaLogger;
  cadena (string) referenciaControladorRobot;

  // Public attribute accessor methods  



  /// 
  /// Set the value of referenciaLogger
  /// @param value the new value of referenciaLogger
  void setReferenciaLogger(cadena (string) value)
  {
    referenciaLogger = value;
  }

  /// 
  /// Get the value of referenciaLogger
  /// @return the value of referenciaLogger
  cadena (string) getReferenciaLogger()
  {
    return referenciaLogger;
  }

  /// 
  /// Set the value of referenciaControladorRobot
  /// @param value the new value of referenciaControladorRobot
  void setReferenciaControladorRobot(cadena (string) value)
  {
    referenciaControladorRobot = value;
  }

  /// 
  /// Get the value of referenciaControladorRobot
  /// @return the value of referenciaControladorRobot
  cadena (string) getReferenciaControladorRobot()
  {
    return referenciaControladorRobot;
  }

  void initAttributes();

};

#endif // GESTORDEREPORTES_H
