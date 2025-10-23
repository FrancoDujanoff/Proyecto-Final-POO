
#ifndef PALOGGER_H
#define PALOGGER_H

#include cadena (string)
#include vector



/// 
/// class PALogger

class PALogger
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  PALogger();

  /// 
  /// Empty Destructor
  virtual ~PALogger();



  /// 
  void info()
  {
  }


  /// 
  void warning()
  {
  }


  /// 
  void getLogFilePath()
  {
  }


  /// 
  /// @param  modulo 
  /// @param  mensaje 
  void logEvento(cadena (string) modulo, cadena (string) mensaje)
  {
  }


  /// 
  /// @param  timestamp 
  /// @param  usuario 
  /// @param  nodo 
  /// @param  peticion 
  /// @param  respuesta 
  void logPeticion(cadena (string) timestamp, cadena (string) usuario, cadena (string) nodo, cadena (string) peticion, cadena (string) respuesta)
  {
  }

private:
  // Private attributes  


  cadena (string) logFIle;

  // Public attribute accessor methods  



  /// 
  /// Set the value of logFIle
  /// @param value the new value of logFIle
  void setLogFIle(cadena (string) value)
  {
    logFIle = value;
  }

  /// 
  /// Get the value of logFIle
  /// @return the value of logFIle
  cadena (string) getLogFIle()
  {
    return logFIle;
  }

  void initAttributes();

};

#endif // PALOGGER_H
