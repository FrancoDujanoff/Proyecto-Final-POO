#ifndef GESTOR_DE_ARCHIVOS_H
#define GESTOR_DE_ARCHIVOS_H

#include <string>
#include <vector>
#include <filesystem> 

//Necesitamos la definición de UsuarioServidor para validar permisos
#include "UsuarioServidor.h"

class GestorDeArchivos {
private:
    
    std::filesystem::path directorioBase; //Ruta base donde se guardarán los archivos G-Code
                                         //std::filesystem::path maneja rutas de forma más robusta


    std::filesystem::path obtenerRutaArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo) const;

    bool crearDirectorioSiNoExiste(const std::filesystem::path& rutaDirectorio);

public:

    //Constructor
    explicit GestorDeArchivos(const std::string& rutaBase); //'explicit' previene conversiones implícitas


    /**
     * @brief Almacena el contenido de un archivo G-Code en el directorio del usuario.
     * @param usuario El usuario que sube el archivo.
     * @param nombreArchivo El nombre deseado para el archivo en el servidor.
     * @param contenido El contenido (G-Code) del archivo como string.
     * @return true si el archivo se guardó con éxito, false en caso contrario.
     * @throw std::runtime_error si ocurre un error grave (ej. no se puede escribir).
     */
    bool almacenarArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo, const std::string& contenido);

    /**
     * @brief Valida si un usuario tiene permiso para acceder a un archivo específico.
     * Los usuarios normales solo acceden a los suyos, el admin a cualquiera.
     * @param usuario El usuario que intenta acceder.
     * @param nombreArchivo El nombre del archivo a verificar.
     * @return true si el usuario tiene permiso, false si no.
     */
    bool validarAccesoArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo) const;

    /**
     * @brief Obtiene el contenido de un archivo G-Code.
     * @param usuario El usuario que solicita el archivo.
     * @param nombreArchivo El nombre del archivo a leer.
     * @return Un string con el contenido del archivo.
     * @throw std::runtime_error si el usuario no tiene permiso o el archivo no se encuentra/puede leer.
     */
    std::string obtenerContenidoArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo);

    /**
     * @brief Lista los nombres de los archivos G-Code disponibles para un usuario.
     * El admin ve todos los archivos, los usuarios normales solo los suyos.
     * @param usuario El usuario para el cual listar los archivos.
     * @return Un vector de strings con los nombres de los archivos.
     * @throw std::runtime_error si ocurre un error al leer el directorio.
     */
    std::vector<std::string> listarArchivos(const UsuarioServidor& usuario) const;

    ~GestorDeArchivos() = default; //Destructor por defecto

}; 

#endif // GESTOR_DE_ARCHIVOS_H
