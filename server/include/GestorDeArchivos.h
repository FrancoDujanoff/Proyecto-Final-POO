#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include <string>
#include "GestorUsuarios.h"ç

class GestorDeArchivos {
private:
    std::string directorioBase;

    /**
     * @brief Valida y construye una ruta de archivo segura.
     * Previene ataques "Path Traversal" (ej. "../..").
     * Devuelve una ruta completa o un string vacío si no es seguro.
     */
    std::string obtenerRutaSegura(const std::string& nombreArchivo) const;

public:
    /**
     * @brief Crea el gestor de archivos.
     * @param dirBase El directorio raíz donde se guardarán todos los archivos.
     */
    GestorDeArchivos(const std::string& dirBase = "server_logs");

    /**
     * @brief Almacena contenido en un archivo.
     * (En esta implementación, 'usuario' no se usa, pero lo mantenemos por el UML)
     */
    bool almacenarArchivo(const Usuario& usuario, const std::string& nombreArchivo, const std.string& contenido);

    /**
     * @brief Valida si un archivo existe y es accesible.
     * (En esta implementación, 'usuario' no se usa)
     */
    bool validarAccesoArchivo(const Usuario& usuario, const std::string& nombreArchivo) const;

    /**
     * @brief Obtiene el contenido completo de un archivo.
     * (En esta implementación, 'usuario' no se usa)
     */
    std::string obtenerContenidoArchivo(const Usuario& usuario, const std::string& nombreArchivo) const;
};

#endif
