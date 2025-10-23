
#ifndef ROBOT_H
#define ROBOT_H

#include cadena (string)


/// 
/// class Robot

class Robot
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  Robot();

  /// 
  /// Empty Destructor
  virtual ~Robot();

private:
  // Private attributes  


  float posicionActual;
  bool estadoMotores;
  bool estadoEfector;
  bool estadoRobot;
  int id;
  cadena (string) modoTrabajo;
  cadena (string) modoCoordenadas;

  // Public attribute accessor methods  



  /// 
  /// Set the value of posicionActual
  /// @param value the new value of posicionActual
  void setPosicionActual(float value)
  {
    posicionActual = value;
  }

  /// 
  /// Get the value of posicionActual
  /// @return the value of posicionActual
  float getPosicionActual()
  {
    return posicionActual;
  }

  /// 
  /// Set the value of estadoMotores
  /// @param value the new value of estadoMotores
  void setEstadoMotores(bool value)
  {
    estadoMotores = value;
  }

  /// 
  /// Get the value of estadoMotores
  /// @return the value of estadoMotores
  bool getEstadoMotores()
  {
    return estadoMotores;
  }

  /// 
  /// Set the value of estadoEfector
  /// @param value the new value of estadoEfector
  void setEstadoEfector(bool value)
  {
    estadoEfector = value;
  }

  /// 
  /// Get the value of estadoEfector
  /// @return the value of estadoEfector
  bool getEstadoEfector()
  {
    return estadoEfector;
  }

  /// 
  /// Set the value of estadoRobot
  /// @param value the new value of estadoRobot
  void setEstadoRobot(bool value)
  {
    estadoRobot = value;
  }

  /// 
  /// Get the value of estadoRobot
  /// @return the value of estadoRobot
  bool getEstadoRobot()
  {
    return estadoRobot;
  }

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
  /// Set the value of modoTrabajo
  /// @param value the new value of modoTrabajo
  void setModoTrabajo(cadena (string) value)
  {
    modoTrabajo = value;
  }

  /// 
  /// Get the value of modoTrabajo
  /// @return the value of modoTrabajo
  cadena (string) getModoTrabajo()
  {
    return modoTrabajo;
  }

  /// 
  /// Set the value of modoCoordenadas
  /// @param value the new value of modoCoordenadas
  void setModoCoordenadas(cadena (string) value)
  {
    modoCoordenadas = value;
  }

  /// 
  /// Get the value of modoCoordenadas
  /// @return the value of modoCoordenadas
  cadena (string) getModoCoordenadas()
  {
    return modoCoordenadas;
  }

  void initAttributes();

};

#endif // ROBOT_H
