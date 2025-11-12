#ifndef PALOGGER_H
#define PALOGGER_H

#include <string>
#include "GestorDeArchivos.h" // Necesario para la dependencia
#include "GestorUsuarios.h"   // Para el tipo 'Usuario'

using namespace std;

// Declaración adelantada, aunque GestorUsuarios.h ya lo incluye
class Usuario;

class PALogger {
private:
    string logFile; // El nombre del archivo (ej: "servidor.log")
    
    // Una referencia al gestor que escribirá en el disco
    GestorDeArchivos* gestorDeArchivos;

    // Usuario "dummy" del sistema para llamar al GestorDeArchivos
    // (recordemos que 'almacenarArchivo' requiere un Usuario)
    Usuario usuarioSistema; 

    /**
     * @brief Formatea y escribe el mensaje final en el archivo.
     * Este es el método privado que hace el trabajo real.
     */
    void log(const string& nivel, const string& mensaje);

public:
    /**
     * @brief Constructor del Logger.
     * @param gestor Un puntero al GestorDeArchivos ya inicializado.
     * @param archivoLog El nombre del archivo donde se guardarán los logs.
     */
    PALogger(GestorDeArchivos* gestor, const string& archivoLog = "servidor.log");

    // --- Métodos del UML ---
    
    /**
     * @brief Registra un mensaje de información.
     */
    void info(const string& mensaje);

    /**
     * @brief Registra un mensaje de advertencia.
     */
    void warning(const string& mensaje);

    /**
     * @brief Registra un evento genérico del sistema.
     */
    void logEvento(const string& modulo, const string& mensaje);

    /**
     * @brief Registra una petición de red (para el futuro).
     */
    void logPeticion(const string& timestamp, const string& usuario, 
                   const string& nodo, const string& peticion, 
                   const string& respuesta);

    /**
     * @brief Devuelve la ruta/nombre del archivo log.
     */
    string getLogFilePath() const;
};

#endif // PALOGGER_H
