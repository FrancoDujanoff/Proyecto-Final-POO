#include "UsuarioServidor.h"
#include <stdexcept> // Para std::invalid_argument
#include <iostream>  // Para advertencias

//Constructor
UsuarioServidor::UsuarioServidor(int idUsuario, const std::string& nombre, const std::string& clavePlana, bool admin)
    : id(idUsuario),
      nombreUsuario(nombre),
      claveHasheada(clavePlana), //Esto es asi debido a que podemos implementar hashing luego que seria seguridad, pero lo consultamos y lo vamos viendo
      esAdministrador(admin)
{
    //Validacion de los parametros

    if (idUsuario < 0) {
      
        throw std::invalid_argument("El ID de usuario no puede ser negativo.");
    }
    if (nombre.empty()) {

        throw std::invalid_argument("El nombre de usuario no puede estar vacío.");
    }
    if (clavePlana.empty()) {

        throw std::invalid_argument("La clave no puede estar vacía.");
    }
    
}

int UsuarioServidor::getId() const {

    return id;
}

const std::string& UsuarioServidor::getNombreUsuario() const {

    return nombreUsuario;
}

bool UsuarioServidor::isAdmin() const {

    return esAdministrador;
}

//Implementacion insegura de verificacion de clave, luego aca se podria modificar con la seguridad

bool UsuarioServidor::verificarClave(const std::string& claveIngresada) const {

    return claveIngresada == claveHasheada; //Compara la clave ingresada con la almacenada (plana)
}

