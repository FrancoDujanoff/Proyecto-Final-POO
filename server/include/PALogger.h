#ifndef PALOGGER_H
#define PALOGGER_H

#include <string>
#include "GestorDeArchivos.h" // Necesario para la dependencia
#include "GestorUsuarios.h"   // Para el tipo 'Usuario'

// Declaración adelantada, aunque GestorUsuarios.h ya lo incluye
class Usuario;

class PALogger {
private:
    std::string logFile; // El nombre del archivo (ej: "servidor.log")
    
    // Una referencia al gestor que escribirá en el disco
    GestorDeArchivos* gestorDeArchivos;

    // Usuario "dummy" del sistema para llamar al GestorDeArchivos
    // (recordemos que 'almacenarArchivo' requiere un Usuario)
    Usuario usuarioSistema; 

    /**
     * @brief Formatea y escribe el mensaje final en el archivo.
     * Este es el método privado que hace el trabajo real.
     */
    void log(const std::string& nivel, const std::string& mensaje);

public:
    /**
     * @brief Constructor del Logger.
     * @param gestor Un puntero al GestorDeArchivos ya inicializado.
     * @param archivoLog El nombre del archivo donde se guardarán los logs.
     */
    PALogger(GestorDeArchivos* gestor, const std::string& archivoLog = "servidor.log");

    // --- Métodos del UML ---
    
    /**
     * @brief Registra un mensaje de información.
     */
    void info(const std::string& mensaje);

    /**
     * @brief Registra un mensaje de advertencia.
     */
    void warning(const std::string& mensaje);

    /**
     * @brief Registra un evento genérico del sistema.
     */
    void logEvento(const std::string& modulo, const std::string& mensaje);

    /**
     * @brief Registra una petición de red (para el futuro).
     */
    void logPeticion(const std::string& timestamp, const std::string& usuario, 
                   const std::string& nodo, const std::string& peticion, 
                   const std::string& respuesta);

    /**
     * @brief Devuelve la ruta/nombre del archivo log.
     */
    std::string getLogFilePath() const;
};

#endif // PALOGGER_H
