#ifndef CONTROLADORROBOT_H
#define CONTROLADORROBOT_H

#include <string>
#include <memory> 
#include <cmath>
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

    string conectarRobot();
    void desconectarRobot();
    string habilitarMotores(bool estado);      
    string activarEfector(bool estado);        
    string moverAPosicion(float x, float y, float z, float velocidad);
    string pausar(float segundos);
    string setModoCoordenadas(bool absoluto);
    string definirPosicionActual(float x, float y, float z, float e = NAN);
    string controlarVentilador(bool estado);
    string solicitarReporteEndstops();
    string enviarComando(const string& comando, int timeout_ms = 30000);
    string solicitarReporteEstadoRobot(); 

    const Robot* getRobot() const { return robot.get(); } 

    string ejecutarBloqueGCode(const string& contenidoGCode);
};

#endif