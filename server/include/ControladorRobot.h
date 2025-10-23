
#ifndef CONTROLADORROBOT_H
#define CONTROLADORROBOT_H

#include cadena (string)
#include vector



/// 
/// class ControladorRobot

class ControladorRobot
{
public:
  // Constructors/Destructors  



  /// 
  /// Empty Constructor
  ControladorRobot();

  /// 
  /// Empty Destructor
  virtual ~ControladorRobot();



  /// 
  void conectarRobot()
  {
  }


  /// 
  void desconectarRobot()
  {
  }


  /// 
  /// @param  estado 
  void habilitarMotores(bool estado)
  {
  }


  /// 
  /// @param  estado 
  void ActivarEfector(bool estado)
  {
  }


  /// 
  /// @param  x 
  /// @param  y 
  /// @param  z 
  /// @param  velocidad 
  void moveAPosicion(float x, float y, float z, float velocidad)
  {
  }


  /// 
  /// @param  comando 
  void enviarComando(cadena (string) comando)
  {
  }


  /// 
  void solicitarReporteEstado()
  {
  }


  /// 
  void ejecutarArchivoGCode()
  {
  }

private:
  // Private attributes  


  Robot Robot;
  PuertoSerie PuertoSerie;
  Robot nuevo_atributo;
  PuertoSerie nuevo_atributo_1;

  // Public attribute accessor methods  



  /// 
  /// Set the value of Robot
  /// @param value the new value of Robot
  void setRobot(Robot value)
  {
    Robot = value;
  }

  /// 
  /// Get the value of Robot
  /// @return the value of Robot
  Robot getRobot()
  {
    return Robot;
  }

  /// 
  /// Set the value of PuertoSerie
  /// @param value the new value of PuertoSerie
  void setPuertoSerie(PuertoSerie value)
  {
    PuertoSerie = value;
  }

  /// 
  /// Get the value of PuertoSerie
  /// @return the value of PuertoSerie
  PuertoSerie getPuertoSerie()
  {
    return PuertoSerie;
  }

  /// 
  /// Set the value of nuevo_atributo
  /// @param value the new value of nuevo_atributo
  void setNuevo_atributo(Robot value)
  {
    nuevo_atributo = value;
  }

  /// 
  /// Get the value of nuevo_atributo
  /// @return the value of nuevo_atributo
  Robot getNuevo_atributo()
  {
    return nuevo_atributo;
  }

  /// 
  /// Set the value of nuevo_atributo_1
  /// @param value the new value of nuevo_atributo_1
  void setNuevo_atributo_1(PuertoSerie value)
  {
    nuevo_atributo_1 = value;
  }

  /// 
  /// Get the value of nuevo_atributo_1
  /// @return the value of nuevo_atributo_1
  PuertoSerie getNuevo_atributo_1()
  {
    return nuevo_atributo_1;
  }

  void initAttributes();

};

#endif // CONTROLADORROBOT_H
