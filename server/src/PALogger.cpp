#include "PALogger.h"
#include <iostream>
#include <sstream>    // Para construir strings
#include <chrono>     // Para la fecha y hora
#include <iomanip>    // Para formatear la fecha y hora

using namespace std;

// Constructor
PALogger::PALogger(GestorDeArchivos* gestor, const string& archivoLog) : logFile(archivoLog), gestorDeArchivos(gestor)
{
    if (gestorDeArchivos == nullptr) {
        throw runtime_error("ERROR: PALogger no puede funcionar sin un GestorDeArchivos.");
    }

    // CAMBIO: Inicializamos el DAO
    usuarioSistema.nombre = "Sistema";
    usuarioSistema.claveHasheada = ""; // Ya no es 'password'
    usuarioSistema.esAdmin = true; 
    
    info("Sistema de LOG inicializado.");
}

/**
 * Método privado que formatea y escribe el log.
 */
void PALogger::log(const string& nivel, const string& mensaje) {
    // 1. Obtener la fecha y hora actual
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    
    // Formato de hora (ej: "2025-11-11 21:17:46")
    stringstream ss_time;
    ss_time << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    string timestamp = ss_time.str();

    // 2. Construir el mensaje de log
    stringstream ss_log;
    ss_log << "[" << timestamp << "] [" << nivel << "] " << mensaje << "\n";
    string logFormateado = ss_log.str();
    
    // 3. Imprimir en la consola (opcional, pero útil)
    cout << logFormateado; 

    // 4. Usar el GestorDeArchivos para guardar
    // Usamos el 'usuarioSistema' que creamos en el constructor.
    gestorDeArchivos->almacenarArchivo(usuarioSistema, logFile, logFormateado);
}

// --- Implementación de los métodos públicos del UML ---

void PALogger::info(const string& mensaje) {
    log("INFO", mensaje);
}

void PALogger::warning(const string& mensaje) {
    log("WARN", mensaje);
}

void PALogger::logEvento(const string& modulo, const string& mensaje) {
    // Formateamos el mensaje para incluir el módulo
    string mensajeCompleto = "[" + modulo + "] " + mensaje;
    log("EVENT", mensajeCompleto);
}

void PALogger::logPeticion(const string& timestamp, const string& usuario, 
                           const string& nodo, const string& peticion, 
                           const string& respuesta) {
    
    // (Ignoramos el timestamp de entrada y generamos uno nuevo)
    string mensajeCompleto = "PETICION: Usuario[" + usuario + "] Nodo[" + nodo + "] "
                                  "Peticion[" + peticion + "] Respuesta[" + respuesta + "]";
    log("NET", mensajeCompleto);
}

string PALogger::getLogFilePath() const {
    return logFile;
}