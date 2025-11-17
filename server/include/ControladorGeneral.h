
#ifndef CONTROLADORGENERAL_H
#define CONTROLADORGENERAL_H

#include <string>
#include <vector>
#include <memory>
#include <sstream> 
#include "ControladorRobot.h"
#include "BaseDeDatos.h"
#include "GestorDeArchivos.h" 
#include "PALogger.h"          
#include "GestorDeReportes.h"

using namespace std;

class ControladorGeneral {
public:
    ControladorGeneral(
        const string& puerto, 
        int baudrate, 
        std::unique_ptr<BaseDeDatos> db // <-- 3b. Cambiar esto
    );
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

    bool estaAccesoRemotoHabilitado() const;

    string guardarArchivoGCode(const string& nombre, const string& contenido);
    string listarArchivosGCode();
    string ejecutarArchivoGCode(const string& nombre);

    // --- MODO APRENDIZAJE ---
    string iniciarAprendizaje();
    string registrarPuntoActual();
    string finalizarAprendizaje(const string& nombreArchivo);

private:
    unique_ptr<ControladorRobot> controladorRobot;
    unique_ptr<BaseDeDatos> gestorDeDatos;
    unique_ptr<GestorDeArchivos> gestorDeArchivos;
    unique_ptr<PALogger> logger;
    unique_ptr<GestorDeReportes> gestorDeReportes;

    bool accesoRemotoHabilitado;

    string archivoLogSistema = "servidor.log";
    string archivoLogActividad = "actividad.log";

    string hashPassword(const string& clavePlana) const;

    bool modoAprendizajeActivo;
    stringstream bufferTrayectoria;
    
};

#endif
