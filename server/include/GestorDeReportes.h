#ifndef GESTORDEREPORTE_H
#define GESTORDEREPORTE_H

#include <string>
#include "GestorDeArchivos.h"
#include "BaseDeDatos.h"

using namespace std;

class ControladorRobot;

class GestorDeReportes {
private:
    string archivoActividad; 

    // Dependencias
    GestorDeArchivos* gestorDeArchivos;
    ControladorRobot* refControladorRobot; // Para reportar el estado del robot

    RegistroUsuarioDAO usuarioSistema; 

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
     */
    void logActividadUsuario(const string& username, const string& accion);

    /**
     * @brief Genera un reporte administrativo (estado actual del robot).
     */
    string generarReporteAdmin();

    /**
     * @brief Obtiene el contenido del log del sistema (servidor.log).
     */
    string generarReporteDeLog(const string& logFileName);
};

#endif