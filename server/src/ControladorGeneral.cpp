#include <iostream>
#include "ControladorGeneral.h"
#include "ControladorRobot.h" 
#include "BaseDeDatos.h"      
#include "GestorDeArchivos.h"  
#include "PALogger.h"          
#include "GestorDeReportes.h"
#include "sha256.h"           
#include <sstream>
#include <cmath>

using namespace std;

// --- IMPLEMENTACIÓN SEGURA DEL HASHER ---
string ControladorGeneral::hashPassword(const string& clavePlana) const {
    // Convertimos a vector para compatibilidad con picosha2
    std::vector<unsigned char> bytes(clavePlana.begin(), clavePlana.end());
    return picosha2::hash256_hex_string(bytes);
}

// --- CONSTRUCTOR ---
ControladorGeneral::ControladorGeneral(const string& puerto, int baudrate, std::unique_ptr<BaseDeDatos> db)
    : gestorDeDatos(std::move(db)), 
      accesoRemotoHabilitado(true)   
{
    gestorDeArchivos = make_unique<GestorDeArchivos>();

    try {
        logger = make_unique<PALogger>(gestorDeArchivos.get(), archivoLogSistema);
    } catch (const exception& e) {
        cerr << "ERROR CRITICO AL INICIAR LOGGER: " << e.what() << endl;
    }

    controladorRobot = make_unique<ControladorRobot>(puerto, baudrate, nullptr);

    try {
        gestorDeReportes = make_unique<GestorDeReportes>(gestorDeArchivos.get(), controladorRobot.get());
    } catch (const exception& e) {
        cerr << "ERROR CRITICO AL INICIAR REPORTES: " << e.what() << endl;
    }

    // Crear admin por defecto si no existe en la BD
    if (!gestorDeDatos->buscarUsuarioPorNombre("admin").has_value()) {
        logger->warning("No se encontró el usuario 'admin'. Creando uno por defecto (admin/admin123).");
        crearNuevoUsuario("admin", "admin123", true);
    }

    logger->info("ControladorGeneral inicializado con BaseDeDatos.");
}

ControladorGeneral::~ControladorGeneral() {}

// --- MÉTODOS DE USUARIO Y SEGURIDAD ---

bool ControladorGeneral::login(const string& usuario, const string& password) {
    auto daoOpt = gestorDeDatos->buscarUsuarioPorNombre(usuario);
    if (!daoOpt.has_value()) {
        return false; 
    }
    
    string hashIngresado = hashPassword(password);
    return (hashIngresado == daoOpt->claveHasheada);
}

bool ControladorGeneral::esAdministrador(const string& usuario) const {
    auto daoOpt = gestorDeDatos->buscarUsuarioPorNombre(usuario);
    if (!daoOpt.has_value()) {
        return false;
    }
    return daoOpt->esAdmin;
}

bool ControladorGeneral::crearNuevoUsuario(const std::string& username, const std::string& password, bool esAdmin) {
    string accion = "Creando usuario: " + username;
    logger->info(accion);
    
    // REGISTRO DE ACTIVIDAD -> ARCHIVO (vía GestorDeReportes)
    if (gestorDeReportes) {
        gestorDeReportes->logActividadUsuario("Sistema/Admin", accion);
    }

    RegistroUsuarioDAO nuevoUsuario;
    nuevoUsuario.nombre = username;
    nuevoUsuario.esAdmin = esAdmin;
    
    // Hashear contraseña
    try {
        nuevoUsuario.claveHasheada = hashPassword(password);
    } catch (const exception& e) {
        logger->warning("Error al hashear password: " + string(e.what()));
        return false;
    }

    if (!gestorDeDatos) return false;

    // PERSISTENCIA DE USUARIO -> MySQL
    return gestorDeDatos->crearUsuario(nuevoUsuario);
}

bool ControladorGeneral::eliminarUnUsuario(const std::string& username) {
    string accion = "Eliminando usuario: " + username;
    logger->info(accion);
    
    // REGISTRO DE ACTIVIDAD -> ARCHIVO
    if (gestorDeReportes) {
        gestorDeReportes->logActividadUsuario("Sistema/Admin", accion);
    }

    if (username == "admin" || esAdministrador(username)) {
         auto lista = gestorDeDatos->listarUsuarios();
         int adminCount = 0;
         for(const auto& u : lista) {
             if (u.esAdmin) adminCount++;
         }
         if (adminCount <= 1) {
             logger->warning("Intento de borrar al último administrador denegado.");
             return false;
         }
    }

    // PERSISTENCIA -> MySQL
    return gestorDeDatos->eliminarUsuarioPorNombre(username);
}

string ControladorGeneral::getListaUsuarios() const {
    stringstream ss;
    ss << "--- Lista de Usuarios (Base de Datos) ---" << std::endl;
    
    auto lista = gestorDeDatos->listarUsuarios();
    
    for (const auto& u : lista) {
        ss << "- " << u.nombre 
           << " (ID: " << u.id 
           << ", Rol: " << (u.esAdmin ? "Admin" : "Usuario") << ")" 
           << std::endl;
    }
    return ss.str();
}

string ControladorGeneral::alternarAccesoRemoto() {
    accesoRemotoHabilitado = !accesoRemotoHabilitado; 
    string estado = accesoRemotoHabilitado ? "HABILITADO" : "DESHABILITADO";
    string accionLog = "El acceso remoto ahora esta: " + estado;

    logger->info(accionLog);
    return "INFO: " + accionLog;
}

// --- RESTO DE MÉTODOS (Sin cambios, solo para referencia) ---

string ControladorGeneral::solicitarReporteEstadoRobot() {
    return controladorRobot->solicitarReporteEstadoRobot();
}

string ControladorGeneral::controlarConexion(bool estado) {
    string respuesta;
    if (estado) { 
        logger->info("Intento de conexion al robot...");
        const Robot* robot = controladorRobot->getRobot();
        if (robot && robot->getEstadoRobot()) return "INFO: El robot ya esta conectado.";
        respuesta = controladorRobot->conectarRobot();
        if (respuesta.find("ERROR") == string::npos) {
            logger->info("Robot conectado exitosamente.");
            gestorDeReportes->logActividadUsuario("Sistema", "Robot conectado.");
        } else {
            logger->warning("Falla al conectar el robot: " + respuesta);
        }
    } else { 
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
    return controladorRobot->enviarComando("G28\r\n");
}

string ControladorGeneral::moverRobot(float x, float y, float z, float velocidad) {
    stringstream ss;
    ss << "Movimiento G1 a X:" << x << " Y:" << y << " Z:" << z;
    logger->info(ss.str());
    gestorDeReportes->logActividadUsuario("Sistema", ss.str());
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

string ControladorGeneral::solicitarReporteAdmin() {
    logger->info("Solicitando Reporte de Administrador.");
    return gestorDeReportes->generarReporteDeLog(archivoLogActividad); 
}

string ControladorGeneral::solicitarReporteLog() {
    logger->info("Solicitando Log del Sistema.");
    return gestorDeReportes->generarReporteDeLog(archivoLogSistema);
}

string ControladorGeneral::shutdownServidor() {
    logger->warning("=== INICIANDO APAGADO DE CONTROLADOR ===");
    if (controladorRobot && controladorRobot->getRobot() && controladorRobot->getRobot()->getEstadoRobot()) {
        logger->info("Apagando robot (M18)...");
        controladorRobot->habilitarMotores(false);
        logger->info("Desconectando puerto serie...");
        controladorRobot->desconectarRobot();
    }
    logger->info("Controlador listo para terminar.");
    return "INFO: Apagado del controlador completado.";
}

const Robot* ControladorGeneral::getRobot() const {
    if (controladorRobot) return controladorRobot->getRobot();
    return nullptr;
}

bool ControladorGeneral::estaAccesoRemotoHabilitado() const {
    return accesoRemotoHabilitado;
}

string ControladorGeneral::guardarArchivoGCode(const string& nombre, const string& contenido) {
    string accion = "Subiendo archivo G-Code: " + nombre;
    logger->info(accion);
    
    if (gestorDeReportes) {
        gestorDeReportes->logActividadUsuario("Sistema", accion); 
    }

    // CORRECCIÓN: Usamos RegistroUsuarioDAO en lugar de Usuario
    RegistroUsuarioDAO dummy; 
    dummy.nombre = "Sistema"; // (Opcional, para que tenga datos)

    if (gestorDeArchivos->almacenarArchivo(dummy, nombre, contenido)) {
        return "INFO: Archivo '" + nombre + "' guardado exitosamente.";
    } else {
        return "ERROR: No se pudo guardar el archivo.";
    }
}

string ControladorGeneral::listarArchivosGCode() {
    logger->info("Listando archivos G-Code disponibles...");
    vector<string> archivos = gestorDeArchivos->listarArchivos();
    
    stringstream ss;
    ss << "Archivos disponibles:\n";
    for (const auto& arch : archivos) {
        ss << " - " << arch << "\n";
    }
    return ss.str();
}

string ControladorGeneral::ejecutarArchivoGCode(const string& nombre) {
    string accion = "Ejecutando archivo G-Code: " + nombre;
    logger->info(accion);
    
    if (gestorDeReportes) {
        gestorDeReportes->logActividadUsuario("Sistema", accion);
    }

    // CORRECCIÓN: Usamos RegistroUsuarioDAO en lugar de Usuario
    RegistroUsuarioDAO dummy;
    dummy.nombre = "Sistema";

    string contenido = gestorDeArchivos->obtenerContenidoArchivo(dummy, nombre);
    
    if (contenido.find("ERROR") != string::npos) {
        return "ERROR: No se pudo leer el archivo " + nombre;
    }

    return controladorRobot->ejecutarBloqueGCode(contenido);
}

string ControladorGeneral::iniciarAprendizaje() {
    modoAprendizajeActivo = true;
    bufferTrayectoria.str(""); // Limpiar buffer
    bufferTrayectoria.clear();
    
    logger->info("Iniciando Modo Aprendizaje de Trayectoria.");
    return "INFO: Modo Aprendizaje ACTIVADO. Mueva el robot y registre puntos.";
}

string ControladorGeneral::registrarPuntoActual() {
    if (!modoAprendizajeActivo) {
        return "ERROR: No esta en modo aprendizaje.";
    }
    
    const Robot* r = controladorRobot->getRobot();
    if (!r) return "ERROR: Estado del robot desconocido.";

    // Formatear como comando G1 (Movimiento lineal)
    stringstream ss;
    ss << "G1 X" << r->getPosX() 
       << " Y" << r->getPosY() 
       << " Z" << r->getPosZ() 
       << " F1000" // Velocidad por defecto segura
       << "\n"; // G-Code requiere salto de línea
       
    bufferTrayectoria << ss.str();
    
    string msg = "Punto registrado: " + ss.str();
    // Quitamos el salto de línea para el log
    if (!msg.empty() && msg.back() == '\n') msg.pop_back(); 
    
    logger->info(msg);
    return "INFO: " + msg;
}

string ControladorGeneral::finalizarAprendizaje(const string& nombreArchivo) {
    if (!modoAprendizajeActivo) {
        return "ERROR: No hay aprendizaje activo para guardar.";
    }

    string contenido = bufferTrayectoria.str();
    if (contenido.empty()) {
        modoAprendizajeActivo = false;
        return "WARN: Trayectoria vacia. Nada guardado.";
    }

    // Usamos el método de guardar archivo que ya creamos
    string resultado = guardarArchivoGCode(nombreArchivo, contenido);
    
    modoAprendizajeActivo = false;
    logger->info("Modo Aprendizaje finalizado. Archivo: " + nombreArchivo);
    
    return resultado;
}