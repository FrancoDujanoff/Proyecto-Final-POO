#ifndef CONTROLADORROBOT_H
#define CONTROLADORROBOT_H

#include <string>
#include <memory> 
#include "Robot.h"
#include "PuertoSerie.h"

using namespace std;

class GestorDeReportes;

class ControladorRobot {
private:
    unique_ptr<Robot> robot; 

    unique_ptr<PuertoSerie> puertoSerie;

    void parsearRespuestaYActualizar(const string& respuesta); 

    GestorDeReportes* gestorDeReportes;

public:
    
    ControladorRobot(const string& puerto, int baudrate, GestorDeReportes* reportes);

    bool conectarRobot();
    void desconectarRobot();

    bool habilitarMotores(bool estado);      
    bool activarEfector(bool estado);        
    bool moverAPosicion(float x, float y, float z, float velocidad); 

    string enviarComando(const string& comando);

    string solicitarReporteEstadoRobot(); 

    const Robot* getRobot() const { return robot.get(); } //Para obtener el estado actual del robot y sin modificarlo

    bool ejecutarBloqueGCode(const string& contenidoGCode);
};

#endif