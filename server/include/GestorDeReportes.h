#ifndef GESTORDEREPORTE_H
#define GESTORDEREPORTE_H

#include <string>
#include "GestorDeArchivos.h"
#include "GestorUsuarios.h" // Para el tipo 'Usuario'

using namespace std;

// Declaración adelantada (Forward declaration)
class ControladorRobot;

class GestorDeReportes {
private:
    string archivoActividad; // ej. "actividad.log"
    // Dependencias
    GestorDeArchivos* gestorDeArchivos;
    ControladorRobot* refControladorRobot; // Para reportar el estado del robot

    Usuario usuarioSistema; // Dummy user para guardar archivos

    /**
     * @brief Escribe una línea de actividad en el log de actividad.
     */
    void registrarActividad(const string& usuario, const string& accion);

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
    void logActividadUsuario(const string& username, const string& accion);

    /**
     * @brief Genera un reporte administrativo (estado actual del robot).
     * (Este es 'generarReporteAdmin' del UML)
     */
    string generarReporteAdmin();

    /**
     * @brief Obtiene el contenido del log del sistema (servidor.log).
     * (Este es 'generarReporteDeLog' del UML)
     */
    string generarReporteDeLog(const string& logFileName);
};

#endif