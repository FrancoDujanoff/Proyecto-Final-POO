#include "GestorDeReportes.h"
#include "ControladorRobot.h" // Necesario para la implementación
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std;

GestorDeReportes::GestorDeReportes(GestorDeArchivos* gestor, ControladorRobot* robot) : archivoActividad("actividad.log"), gestorDeArchivos(gestor), refControladorRobot(robot)
{
    if (gestorDeArchivos == nullptr || refControladorRobot == nullptr) {
        throw runtime_error("ERROR: GestorDeReportes requiere GestorDeArchivos y ControladorRobot.");
    }

    // CAMBIO: Inicialización del DAO
    usuarioSistema.nombre = "Sistema";
    usuarioSistema.claveHasheada = "";
    usuarioSistema.esAdmin = true;

    logActividadUsuario("Sistema", "Gestor de Reportes inicializado.");
}

/**
 * Método privado que formatea y escribe el log de actividad.
 */
void GestorDeReportes::registrarActividad(const string& usuario, const string& accion) {
    // 1. Obtener la fecha y hora actual
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    stringstream ss_time;
    ss_time << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    // 2. Construir el mensaje de log
    stringstream ss_log;
    ss_log << "[" << ss_time.str() << "] [Usuario: " << usuario << "] " << accion << "\n";
    string logFormateado = ss_log.str();

    // 3. Usar el GestorDeArchivos para guardar
    gestorDeArchivos->almacenarArchivo(usuarioSistema, archivoActividad, logFormateado);
}

// --- Implementación de los métodos públicos ---

void GestorDeReportes::logActividadUsuario(const string& username, const string& accion) {
    registrarActividad(username, accion);
}

string GestorDeReportes::generarReporteAdmin() {
    // Esta función ahora simplemente obtiene el estado del robot.
    // Asumimos que el robot está conectado.
    try {
        return refControladorRobot->solicitarReporteEstadoRobot();
    } catch (const exception& e) {
        return "ERROR al generar reporte del robot: " + string(e.what());
    }
}

string GestorDeReportes::generarReporteDeLog(const string& logFileName) {
    // Esta función lee un archivo de log del disco usando el GestorDeArchivos
    return gestorDeArchivos->obtenerContenidoArchivo(usuarioSistema, logFileName);
}