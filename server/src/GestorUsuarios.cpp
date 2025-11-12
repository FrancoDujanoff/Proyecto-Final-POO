#include "GestorUsuarios.h"
#include <fstream>   // Para leer/escribir archivos (ifstream, ofstream)
#include <sstream>   // Para parsear líneas (stringstream)
#include <iostream>  // Para logs de error

GestorUsuarios::GestorUsuarios(const std::string& archivo) : archivoDeUsuarios(archivo) 
{
    cargarUsuariosDesdeArchivo();

    // Si el archivo estaba vacío, creamos un admin por defecto
    if (mapaUsuarios.empty()) {
        std::cout << "INFO: No se encontraron usuarios. Creando 'admin' por defecto..." << std::endl;
        crearUsuario("admin", "admin123", true);
        // 'crearUsuario' ya llama a 'guardarUsuariosEnArchivo'
    }
}

// El destructor se asegura de guardar todo al salir
GestorUsuarios::~GestorUsuarios() {
    // Podríamos optimizar esto para guardar solo si hay cambios,
    // pero por ahora, guardamos siempre para asegurar consistencia.
    // guardarUsuariosEnArchivo(); 
}

bool GestorUsuarios::validarUsuario(const std::string& username, const std::string& password) {
    // map::find es muy rápido
    auto it = mapaUsuarios.find(username);
    
    if (it == mapaUsuarios.end()) {
        return false; // Usuario no encontrado
    }
    
    // Comparamos la contraseña
    return it->second.password == password;
}

bool GestorUsuarios::esAdministrador(const std::string& username) {
    // Usamos 'at' para obtener el usuario (lanza excepción si no existe)
    // Asumimos que esto se llama *después* de validarUsuario
    try {
        return mapaUsuarios.at(username).esAdmin;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

bool GestorUsuarios::crearUsuario(const std::string& username, const std::string& password, bool esAdmin) {
    // Verificamos si el usuario ya existe
    if (mapaUsuarios.find(username) != mapaUsuarios.end()) {
        std::cerr << "ERROR: El usuario '" << username << "' ya existe." << std::endl;
        return false;
    }

    mapaUsuarios[username] = {password, esAdmin};
    
    guardarUsuariosEnArchivo();
    std::cout << "INFO: Usuario '" << username << "' creado." << std::endl;
    return true;
}

bool GestorUsuarios::eliminarUsuario(const std::string& username) {
    auto it = mapaUsuarios.find(username);
    if (it == mapaUsuarios.end()) {
        std::cerr << "ERROR: El usuario '" << username << "' no existe." << std::endl;
        return false;
    }

    // Regla de negocio: No permitir borrar al último administrador
    if (it->second.esAdmin) {
        int adminCount = 0;
        for (const auto& par : mapaUsuarios) {
            if (par.second.esAdmin) adminCount++;
        }
        if (adminCount <= 1) {
            std::cerr << "ERROR: No se puede eliminar al ultimo administrador." << std::endl;
            return false;
        }
    }

    mapaUsuarios.erase(it);
    guardarUsuariosEnArchivo();
    std::cout << "INFO: Usuario '" << username << "' eliminado." << std::endl;
    return true;
}

std::string GestorUsuarios::listarUsuarios() const {
    std::stringstream ss;
    ss << "--- Lista de Usuarios ---" << std::endl;
    for (const auto& par : mapaUsuarios) {
        ss << "- " << par.first 
           << " (Rol: " << (par.second.esAdmin ? "Admin" : "Usuario") << ")" 
           << std::endl;
    }
    return ss.str();
}

// --- MÉTODOS PRIVADOS DE ARCHIVO ---

void GestorUsuarios::cargarUsuariosDesdeArchivo() {
    std::ifstream archivo(archivoDeUsuarios);
    if (!archivo.is_open()) {
        std::cerr << "WARN: No se encontró el archivo '" << archivoDeUsuarios << "'. Se creará uno nuevo." << std::endl;
        return;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string username, password, strEsAdmin;
        
        // Formato CSV: username,password,esAdmin(1 o 0)
        if (std::getline(ss, username, ',') &&
            std::getline(ss, password, ',') &&
            std::getline(ss, strEsAdmin)) 
        {
            mapaUsuarios[username] = {password, (strEsAdmin == "1")};
        }
    }
    archivo.close();
}

void GestorUsuarios::guardarUsuariosEnArchivo() {
    std::ofstream archivo(archivoDeUsuarios);
    if (!archivo.is_open()) {
        std::cerr << "ERROR: No se pudo abrir '" << archivoDeUsuarios << "' para guardar." << std::endl;
        return;
    }
    for (const auto& par : mapaUsuarios) {
        archivo << par.first << "," 
                << par.second.password << "," 
                << (par.second.esAdmin ? "1" : "0") 
                << "\n";
    }
    archivo.close();
} 