
#ifndef CONTROLADORGENERAL_H
#define CONTROLADORGENERAL_H

#include <string>
#include <vector>
#include <memory> 
#include "ControladorRobot.h"
#include "GestorUsuarios.h"
#include "GestorDeArchivos.h" 
#include "PALogger.h"          
#include "GestorDeReportes.h"

using namespace std;

class ControladorGeneral {
public:
    ControladorGeneral(const string& puerto, int baudrate, const string& archivoUsuarios);
    virtual ~ControladorGeneral();

    bool login(const string& usuario, const string& password);
    bool esAdministrador(const string& usuario) const;
    bool crearNuevoUsuario(const string& username, const string& password, bool esAdmin);
    bool eliminarUnUsuario(const string& username);
    string getListaUsuarios() const;

    string solicitarReporteEstadoRobot();
    string controlarConexion(bool estado);
    string controlarMotores(bool estado);
    string irAHome();
    string moverRobot(float x, float y, float z, float velocidad);
    string controlarEfector(bool estado);

    string pausar(float segundos);
    string setModoCoordenadas(bool absoluto);
    string definirPosicionActual(float x, float y, float z, float e = NAN);
    string controlarVentilador(bool estado);
    string solicitarReporteEndstops();

    string alternarAccesoRemoto();//Es para la opcion 2 que todavia no esta implemtada
    string solicitarReporteAdmin();
    string solicitarReporteLog();
    string shutdownServidor();

    const Robot* getRobot() const;

private:
    unique_ptr<ControladorRobot> controladorRobot;
    unique_ptr<GestorUsuarios> gestorUsuarios; 
    unique_ptr<GestorDeArchivos> gestorDeArchivos;
    unique_ptr<PALogger> logger;
    unique_ptr<GestorDeReportes> gestorDeReportes;

    string archivoLogSistema = "servidor.log";
    string archivoLogActividad = "actividad.log";
};

#endif
