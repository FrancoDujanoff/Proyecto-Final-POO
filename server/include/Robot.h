#ifndef ROBOT_H
#define ROBOT_H

#include <string>

using namespace std;

class Robot {
private:
    
    float posX;
    float posY;
    float posZ;

    bool estadoMotores;      
    bool estadoEfector; 
    bool estadoRobot;

    int id;                 
    string modoTrabajo;      // Ej: "MANUAL", "AUTOMATICO"
    string modoCoordenadas;  // Ej: "ABSOLUTO", "RELATIVO"

public:
    
    Robot(); 
    
    float getPosX() const { return posX; } //Necesarios para poder tomar el estado actual
    float getPosY() const { return posY; }
    float getPosZ() const { return posZ; }
    bool getEstadoMotores() const { return estadoMotores; }
    bool getEstadoEfector() const { return estadoEfector; }
    bool getEstadoRobot() const { return estadoRobot; }
    int getId() const { return id; }
    string getModoTrabajo() const { return modoTrabajo; }
    string getModoCoordenadas() const { return modoCoordenadas; }

    void setPosX(float x) { posX = x; } //Necesarios para actualizar el estado
    void setPosY(float y) { posY = y; }
    void setPosZ(float z) { posZ = z; }
    void setEstadoMotores(bool estado) { estadoMotores = estado; }
    void setEstadoEfector(bool estado) { estadoEfector = estado; }
    void setEstadoRobot(bool estado) { estadoRobot = estado; }
    void setModoTrabajo(const string& modo) { modoTrabajo = modo; }
    void setModoCoordenadas(const string& modo) { modoCoordenadas = modo; }
};

#endif
