#ifndef ROBOT_H
#define ROBOT_H

#include <string>

using namespace std;

class Robot {
private:
    
    float posX;
    float posY;
    float posZ;
    float ultimaVelocidad;

    bool estadoMotores;      
    bool estadoEfector; 
    bool estadoRobot;
    bool estadoVentilador;

    int id;                 
    string modoCoordenadas;

public:
    
    Robot(); 

    //Necesarios para poder tomar el estado actual
    float getPosX() const { return posX; } 
    float getPosY() const { return posY; }
    float getPosZ() const { return posZ; }

    bool getEstadoMotores() const { return estadoMotores; }
    bool getEstadoEfector() const { return estadoEfector; }
    bool getEstadoRobot() const { return estadoRobot; }
    int getId() const { return id; }
    bool getEstadoVentilador() const { return estadoVentilador; }
    string getModoCoordenadas() const { return modoCoordenadas; }
    float getUltimaVelocidad() const { return ultimaVelocidad; }
    
    //Necesarios para actualizar el estado
    void setPosX(float x) { posX = x; } 
    void setPosY(float y) { posY = y; }
    void setPosZ(float z) { posZ = z; }
    void setUltimaVelocidad(float v) { ultimaVelocidad = v; }
    void setEstadoMotores(bool estado) { estadoMotores = estado; }
    void setEstadoEfector(bool estado) { estadoEfector = estado; }
    void setEstadoRobot(bool estado) { estadoRobot = estado; }
    void setEstadoVentilador(bool estado) { estadoVentilador = estado; }
    void setModoCoordenadas(const string& modo) { modoCoordenadas = modo; }
};

#endif
