
#include <iostream>
#include "ControladorGeneral.h"
#include "ControladorRobot.h" 
#include "GestorUsuarios.h"
#include "GestorDeArchivos.h"  
#include "PALogger.h"          
#include "GestorDeReportes.h"
#include <sstream>
#include <cmath>

using namespace std;

ControladorGeneral::ControladorGeneral(const string& puerto, int baudrate, const string& archivoUsuarios)
{
    // 1. Instanciamos GestorDeUsuarios
    gestorUsuarios = make_unique<GestorUsuarios>(archivoUsuarios);

    // 2. Instanciamos el GestorDeArchivos (usará "server_logs" por defecto)
    gestorDeArchivos = make_unique<GestorDeArchivos>();

    // 3. Instanciamos el PALogger (le pasamos el gestor de archivos)
    try {
        logger = make_unique<PALogger>(gestorDeArchivos.get(), archivoLogSistema);
    } catch (const exception& e) {
        cerr << "ERROR CRITICO AL INICIAR LOGGER: " << e.what() << endl;
        // El programa podría terminar aquí si el logger es esencial
    }

    // 4. Instanciamos el ControladorRobot
    controladorRobot = make_unique<ControladorRobot>(puerto, baudrate, nullptr);

    // 5. Instanciamos GestorDeReportes (le pasamos el gestor de archivos y el robot)
    try {
        gestorDeReportes = make_unique<GestorDeReportes>(gestorDeArchivos.get(), controladorRobot.get());
    } catch (const exception& e) {
        cerr << "ERROR CRITICO AL INICIAR REPORTES: " << e.what() << endl;
    }

    logger->info("ControladorGeneral inicializado.");
}
ControladorGeneral::~ControladorGeneral() {}

bool ControladorGeneral::login(const string& usuario, const string& password) {
    return gestorUsuarios->validarUsuario(usuario, password);
}

bool ControladorGeneral::esAdministrador(const string& usuario) const {
    return gestorUsuarios->esAdministrador(usuario);
}

bool ControladorGeneral::crearNuevoUsuario(const std::string& username, const std::string& password, bool esAdmin) {
    // --- AÑADIR LOGS ---
    std::string accion = "Creando usuario: " + username;
    logger->info(accion);
    // (Asumimos que el admin actual es "Sistema" por ahora)
    gestorDeReportes->logActividadUsuario("Sistema/Admin", accion);
    // --- FIN DE LOGS ---
    
    return gestorUsuarios->crearUsuario(username, password, esAdmin);
}

bool ControladorGeneral::eliminarUnUsuario(const std::string& username) {
    // --- AÑADIR LOGS ---
    std::string accion = "Eliminando usuario: " + username;
    logger->info(accion);
    gestorDeReportes->logActividadUsuario("Sistema/Admin", accion);
    // --- FIN DE LOGS ---

    return gestorUsuarios->eliminarUsuario(username);
}

string ControladorGeneral::getListaUsuarios() const {
    return gestorUsuarios->listarUsuarios();
}

string ControladorGeneral::solicitarReporteEstadoRobot() {
    return controladorRobot->solicitarReporteEstadoRobot();
}

string ControladorGeneral::controlarConexion(bool estado) {
    string respuesta;
    
    if (estado == true) { // Conectar
        logger->info("Intento de conexion al robot...");
        const Robot* robot = controladorRobot->getRobot();
        if (robot && robot->getEstadoRobot()) {
            return "INFO: El robot ya esta conectado.";
        }
        
        respuesta = controladorRobot->conectarRobot();
        
        if (respuesta.find("ERROR") == string::npos) {
            logger->info("Robot conectado exitosamente.");
            gestorDeReportes->logActividadUsuario("Sistema", "Robot conectado.");
        } else {
            logger->warning("Falla al conectar el robot: " + respuesta);
        }
    } else { // Desconectar
        logger->info("Desconectando el robot...");
        controladorRobot->desconectarRobot();
        respuesta = "INFO: Robot Desconectado.";
        gestorDeReportes->logActividadUsuario("Sistema", "Robot desconectado.");
    }
    return respuesta;
}

string ControladorGeneral::controlarMotores(bool estado) {
    string accion = estado ? "Habilitar Motores" : "Deshabilitar Motores";
    logger->info(accion);
    gestorDeReportes->logActividadUsuario("Sistema", accion);
    
    return controladorRobot->habilitarMotores(estado);
}

string ControladorGeneral::irAHome() {
    logger->info("Enviando comando Home (G28)...");
    gestorDeReportes->logActividadUsuario("Sistema", "Comando Home (G28).");
    // Usamos el timeout por defecto de 30s de enviarComando
    return controladorRobot->enviarComando("G28\r\n");

}

string ControladorGeneral::moverRobot(float x, float y, float z, float velocidad) {
    stringstream ss;
    ss << "Movimiento G1 a X:" << x << " Y:" << y << " Z:" << z;
    string accion = ss.str();
    
    logger->info(accion);
    gestorDeReportes->logActividadUsuario("Sistema", accion);

    return controladorRobot->moverAPosicion(x, y, z, velocidad);
}

string ControladorGeneral::controlarEfector(bool estado) {
    string accion = estado ? "Activar Efector" : "Desactivar Efector";
    logger->info(accion);
    gestorDeReportes->logActividadUsuario("Sistema", accion);

    return controladorRobot->activarEfector(estado);
}

string ControladorGeneral::pausar(float segundos) {
    logger->info("Comando Pausa (G4) ejecutado - (No implementado).");
    return controladorRobot->pausar(segundos);
}

string ControladorGeneral::setModoCoordenadas(bool absoluto) {
    string accion = absoluto ? "Modo Absoluto (G90)" : "Modo Relativo (G91)";
    logger->info(accion);
    return controladorRobot->setModoCoordenadas(absoluto);
}

string ControladorGeneral::definirPosicionActual(float x, float y, float z, float e) {
    logger->info("Definiendo posicion actual (G92)...");
    return controladorRobot->definirPosicionActual(x, y, z, e);
}

string ControladorGeneral::controlarVentilador(bool estado) {
    string accion = estado ? "Encender Ventilador (M106)" : "Apagar Ventilador (M107)";
    logger->info(accion);
    return controladorRobot->controlarVentilador(estado);
}

string ControladorGeneral::solicitarReporteEndstops() {
    logger->info("Solicitando reporte de Endstops (M119)...");
    return controladorRobot->solicitarReporteEndstops();
}

// Todavia no implemteado
string ControladorGeneral::alternarAccesoRemoto() {
    logger->info("Llamada a funcion stub: alternarAccesoRemoto");
    cout << "INFO: Acceso Remoto (No implementado)" << endl;
    return "INFO: Acceso Remoto (No implementado)";
}

string ControladorGeneral::solicitarReporteAdmin() {
    logger->info("Solicitando Reporte de Administrador.");
    return gestorDeReportes->generarReporteDeLog(archivoLogActividad); 
}

string ControladorGeneral::solicitarReporteLog() {
    logger->info("Solicitando Log del Sistema.");
    return gestorDeReportes->generarReporteDeLog(archivoLogSistema);
}

string ControladorGeneral::shutdownServidor() {
    logger->info("Llamada a funcion stub: shutdownServidor");
    cout << "INFO: Shutdown (No implementado)" << endl; 
    return "INFO: Shutdown (No implementado)";
}

const Robot* ControladorGeneral::getRobot() const {
    if (controladorRobot) {
        return controladorRobot->getRobot();
    }
    return nullptr;
}
