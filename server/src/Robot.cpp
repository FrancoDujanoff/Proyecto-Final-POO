#include "Robot.h"

//Definimos valores iniciales para el robot
Robot::Robot() 
    : posX(0.0f), posY(0.0f), posZ(0.0f), // Posición inicial, por ejemplo el Home
      estadoMotores(false),               // M18: Desactivados
      estadoEfector(false),               // M5: Pinza Desactivada
      estadoRobot(false),                 // No conectado/activo
      id(1),                                 
      modoTrabajo("MANUAL"),                 
      modoCoordenadas("ABSOLUTO") {}
