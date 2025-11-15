#include "Robot.h"

//Definimos valores iniciales para el robot, pero que no necesariamente son los que estan cargados en el Arduino
Robot::Robot() 
    : posX(0.0f), posY(0.0f), posZ(0.0f), 
      ultimaVelocidad(0.0f),
      estadoMotores(false),               
      estadoEfector(false),               
      estadoRobot(false),    
      estadoVentilador(false),           
      id(1),                                                 
      modoCoordenadas("ABSOLUTO") {}
