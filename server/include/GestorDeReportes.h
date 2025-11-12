#ifndef GESTORDEREPORTE_H
#define GESTORDEREPORTE_H

#include <string>
#include "GestorDeArchivos.h"
#include "GestorUsuarios.h" // Para el tipo 'Usuario'

// Declaración adelantada (Forward declaration)
class ControladorRobot;

class GestorDeReportes {
private:
    std::string archivoActividad; // ej. "actividad.log"
    std
    // Dependencias
    GestorDeArchivos* gestorDeArchivos;
    ControladorRobot* refControladorRobot; // Para reportar el estado del robot

    Usuario usuarioSistema; // Dummy user para guardar archivos

    /**
     * @brief Escribe una línea de actividad en el log de actividad.
     */
    void registrarActividad(const std::string& usuario, const std::string& accion);

public:
    /**
     * @brief Constructor del GestorDeReportes.
     * @param gestor Un puntero al GestorDeArchivos.
     * @param robot Un puntero al ControladorRobot.
     */
    GestorDeReportes(GestorDeArchivos* gestor, ControladorRobot* robot);

    // --- Métodos del UML (adaptados) ---

    /**
     * @brief Registra una acción realizada por un usuario.
     * (Este reemplaza 'generarReporteActividad(Usuario)' del UML)
     */
    void logActividadUsuario(const std::string& username, const std::string& accion);

    /**
     * @brief Genera un reporte administrativo (estado actual del robot).
     * (Este es 'generarReporteAdmin' del UML)
     */
    std::string generarReporteAdmin();

    /**
     * @brief Obtiene el contenido del log del sistema (servidor.log).
     * (Este es 'generarReporteDeLog' del UML)
     */
    std::string generarReporteDeLog(const std::string& logFileName);
};

#endif