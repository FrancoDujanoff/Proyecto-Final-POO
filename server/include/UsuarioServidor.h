#ifndef USUARIO_SERVIDOR_H
#define USUARIO_SERVIDOR_H

#include <string> // Necesitamos std::string para el nombre y la clave (hash)

class UsuarioServidor {
private:

    int id;                     // Identificador único de la base de datos
    std::string nombreUsuario;  // Nombre de login del usuario
    std::string claveHasheada; // IMPORTANTE: Hash de la clave, NUNCA la clave en texto plano
    bool esAdministrador;       // Flag para indicar si es administrador o no

public:

    //Constructor Principal
    UsuarioServidor(int idUsuario, const std::string& nombre, const std::string& hashClave, bool admin);

    //Metodos

    int getId() const;

    const std::string& getNombreUsuario() const; //Devolvemos const& para eficiencia

    bool isAdmin() const;
    
    bool verificarClave(const std::string& claveIngresada) const;// Método para verificar la clave ingresada contra el hash almacenado

    ~UsuarioServidor() = default; //Usamos el destructor por defecto

}; 

#endif // USUARIO_SERVIDOR_H