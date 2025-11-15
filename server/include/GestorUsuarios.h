#ifndef GESTORUSUARIOS_H
#define GESTORUSUARIOS_H

#include <string>
#include <map>

// Una estructura simple para guardar los datos de cada usuario en memoria
struct Usuario {
    std::string password;
    bool esAdmin;
};

class GestorUsuarios {
private:
    // Un mapa para acceso rápido. La clave es el username (string).
    std::map<std::string, Usuario> mapaUsuarios;
    
    // El archivo donde guardamos/cargamos los usuarios
    std::string archivoDeUsuarios;

    // Métodos privados para manejar el archivo
    void cargarUsuariosDesdeArchivo();
    void guardarUsuariosEnArchivo();

public:
    // El constructor recibe el nombre del archivo de usuarios
    GestorUsuarios(const std::string& archivo);
    
    // El destructor guardará los cambios al cerrar
    ~GestorUsuarios();

    // --- Métodos Requeridos por ControladorGeneral ---
    
    // Verifica si el usuario y la contraseña coinciden
    bool validarUsuario(const std::string& username, const std::string& password);
    
    // Verifica si el usuario tiene rol de administrador
    bool esAdministrador(const std::string& username);

    // --- Métodos Requeridos por AdminCLI (Opción 5) ---
    
    // Añade un nuevo usuario al sistema
    bool crearUsuario(const std::string& username, const std::string& password, bool esAdmin);
    
    // (Opcional) Elimina un usuario
    bool eliminarUsuario(const std::string& username);

    // (Opcional) Un método para listar usuarios
    std::string listarUsuarios() const;
};

#endif