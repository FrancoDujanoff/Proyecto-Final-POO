
#ifndef ADMINCLI_H
#define ADMINCLI_H

#include cadena (string)
#include vector



/// 
/// class AdminCLI

class AdminCLI
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  AdminCLI();

  /// 
  /// Empty Destructor
  virtual ~AdminCLI();



  /// 
  void iniciarSesion()
  {
  }


  /// 
  void mostrarMenu()
  {
  }


  /// 
  void procesarEntrada()
  {
  }

private:
  // Private attributes  


  cadena (string) referenciaControladorGeneral;

  // Public attribute accessor methods  



  /// 
  /// Set the value of referenciaControladorGeneral
  /// @param value the new value of referenciaControladorGeneral
  void setReferenciaControladorGeneral(cadena (string) value)
  {
    referenciaControladorGeneral = value;
  }

  /// 
  /// Get the value of referenciaControladorGeneral
  /// @return the value of referenciaControladorGeneral
  cadena (string) getReferenciaControladorGeneral()
  {
    return referenciaControladorGeneral;
  }

  void initAttributes();

};

#endif // ADMINCLI_H
