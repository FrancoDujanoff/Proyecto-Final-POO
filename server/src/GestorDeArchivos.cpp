#include "GestorDeArchivos.h"
#include "UsuarioServidor.h" 
#include <fstream>        //Para std::ofstream (escritura) y std::ifstream (lectura)
#include <stdexcept>      //Para lanzar excepciones (ej. std::runtime_error)
#include <system_error>   //Para capturar errores de std::filesystem
#include <iostream>       //Para posible logging de errores (mejor usar PALogger)


GestorDeArchivos::GestorDeArchivos(const std::string& rutaBase) {
    try {
        directorioBase = std::filesystem::absolute(rutaBase); // Convertir a ruta absoluta

        // Crear el directorio base si no existe
        if (!std::filesystem::exists(directorioBase)) {
            if (!std::filesystem::create_directories(directorioBase)) {

                // Si no se pudo crear, lanzamos una excepción
                throw std::runtime_error("No se pudo crear el directorio base: " + directorioBase.string());
            }

            std::cout << "Directorio base creado en: " << directorioBase.string() << std::endl;
        } else if (!std::filesystem::is_directory(directorioBase)) {
          
            // Si existe pero no es un directorio, es un error
            throw std::runtime_error("La ruta base especificada existe pero no es un directorio: " + directorioBase.string());
        }
    } catch (const std::filesystem::filesystem_error& e) {

        throw std::runtime_error("Error de filesystem al inicializar GestorDeArchivos: " + std::string(e.what()));
    }
}

// Construye la ruta completa para un archivo de un usuario específico
std::filesystem::path GestorDeArchivos::obtenerRutaArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo) const {
    // Usamos el nombre de usuario para crear un subdirectorio (¡asegúrate de que los nombres de usuario sean válidos para directorios!)
    std::filesystem::path rutaUsuario = directorioBase / usuario.getNombreUsuario();
    return rutaUsuario / nombreArchivo; // Sobrecarga del operador / para unir rutas
}

// Crea un directorio si no existe (método auxiliar)
bool GestorDeArchivos::crearDirectorioSiNoExiste(const std::filesystem::path& rutaDirectorio) {
    try {
        if (!std::filesystem::exists(rutaDirectorio)) {
            if (!std::filesystem::create_directories(rutaDirectorio)) {
                std::cerr << "Error: No se pudo crear el directorio: " << rutaDirectorio.string() << std::endl;
                return false;
            }
            // Log: Directorio creado
        } else if (!std::filesystem::is_directory(rutaDirectorio)) {
            std::cerr << "Error: La ruta ya existe pero no es un directorio: " << rutaDirectorio.string() << std::endl;
            return false;
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error de filesystem al crear/verificar directorio " << rutaDirectorio.string() << ": " << e.what() << std::endl;
        return false;
    }
}

// --- Métodos Principales ---

bool GestorDeArchivos::almacenarArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo, const std::string& contenido) {
    std::filesystem::path rutaCompleta = obtenerRutaArchivo(usuario, nombreArchivo);
    std::filesystem::path rutaDirectorioUsuario = rutaCompleta.parent_path();

    // 1. Asegurarse de que el directorio del usuario exista
    if (!crearDirectorioSiNoExiste(rutaDirectorioUsuario)) {
        return false; // El error ya se imprimió en crearDirectorioSiNoExiste
    }

    // 2. Abrir el archivo para escritura (std::ofstream)
    //    std::ios::trunc asegura que si el archivo existe, se sobrescribe.
    std::ofstream archivoSalida(rutaCompleta, std::ios::out | std::ios::trunc);

    if (!archivoSalida.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para escritura: " << rutaCompleta.string() << std::endl;
        return false;
    }

    // 3. Escribir el contenido
    archivoSalida << contenido;

    // 4. Verificar errores de escritura (opcional pero recomendado)
    if (archivoSalida.fail()) {
         std::cerr << "Error: Ocurrió un error durante la escritura en: " << rutaCompleta.string() << std::endl;
         archivoSalida.close(); // Intentar cerrar aunque haya fallado
         return false;
    }

    // 5. Cerrar el archivo (se cierra automáticamente al salir del alcance, pero es buena práctica ser explícito si verificas errores)
    archivoSalida.close();

    // Log: Archivo almacenado con éxito
    return true;
}

bool GestorDeArchivos::validarAccesoArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo) const {
    // Si el usuario es admin, puede acceder a cualquier archivo (asumimos formato "usuario/archivo.gcode")
    if (usuario.isAdmin()) {
        // Buscamos si el nombreArchivo incluye un separador, indicando ruta de otro usuario
        size_t separador = nombreArchivo.find('/'); // O '\\' en Windows si fuera necesario
        std::filesystem::path rutaCompleta;
        if (separador != std::string::npos) {
             rutaCompleta = directorioBase / nombreArchivo; // Admin especificó usuario/archivo
        } else {
             // Si el admin no especifica usuario, ¿asumimos el suyo o buscamos en todos?
             // Por simplicidad aquí, asumimos que debe especificar "usuario/archivo" para acceder a otros.
             // O podríamos buscar en todos los directorios. Decidimos buscar solo en el suyo si no especifica.
             rutaCompleta = obtenerRutaArchivo(usuario, nombreArchivo);
             // Alternativa: buscar en todos los subdirectorios... más complejo.
        }
         return std::filesystem::exists(rutaCompleta) && std::filesystem::is_regular_file(rutaCompleta);
    } else {
        // Usuario normal: solo puede acceder a archivos en su propio directorio
        std::filesystem::path rutaCompleta = obtenerRutaArchivo(usuario, nombreArchivo);
        return std::filesystem::exists(rutaCompleta) && std::filesystem::is_regular_file(rutaCompleta);
    }
}

std::string GestorDeArchivos::obtenerContenidoArchivo(const UsuarioServidor& usuario, const std::string& nombreArchivo) {
    // 1. Validar permiso primero
    if (!validarAccesoArchivo(usuario, nombreArchivo)) {
        throw std::runtime_error("Acceso denegado o archivo no encontrado para: " + nombreArchivo);
    }

    // Determinar la ruta correcta (puede variar si es admin accediendo a otro usuario)
     std::filesystem::path rutaCompleta;
     if (usuario.isAdmin() && nombreArchivo.find('/') != std::string::npos) {
         rutaCompleta = directorioBase / nombreArchivo;
     } else {
         rutaCompleta = obtenerRutaArchivo(usuario, nombreArchivo);
     }


    // 2. Abrir el archivo para lectura (std::ifstream)
    std::ifstream archivoEntrada(rutaCompleta);
    if (!archivoEntrada.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo para lectura: " + rutaCompleta.string());
    }

    // 3. Leer todo el contenido en un string
    std::stringstream buffer;
    buffer << archivoEntrada.rdbuf(); // Lee todo el stream buffer al stringstream
    archivoEntrada.close();

    if (archivoEntrada.fail() && !archivoEntrada.eof()) { // Verifica error *después* de leer/cerrar, excepto EOF
        throw std::runtime_error("Error durante la lectura del archivo: " + rutaCompleta.string());
    }

    return buffer.str();
}

std::vector<std::string> GestorDeArchivos::listarArchivos(const UsuarioServidor& usuario) const {
    std::vector<std::string> listaArchivos;

    if (usuario.isAdmin()) {
        // Admin: Listar todos los archivos en todos los subdirectorios de usuario
        try {
            for (const auto& entryUsuario : std::filesystem::directory_iterator(directorioBase)) {
                if (entryUsuario.is_directory()) {
                    std::string nombreUsuarioDir = entryUsuario.path().filename().string();
                    for (const auto& entryArchivo : std::filesystem::directory_iterator(entryUsuario.path())) {
                        if (entryArchivo.is_regular_file()) {
                            // Guardamos en formato "usuario/archivo" para el admin
                            listaArchivos.push_back(nombreUsuarioDir + "/" + entryArchivo.path().filename().string());
                        }
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
             throw std::runtime_error("Error de filesystem al listar archivos para admin: " + std::string(e.what()));
        }
    } else {
        // Usuario Normal: Listar solo archivos en su directorio
        std::filesystem::path rutaDirectorioUsuario = directorioBase / usuario.getNombreUsuario();
        try {
            if (std::filesystem::exists(rutaDirectorioUsuario) && std::filesystem::is_directory(rutaDirectorioUsuario)) {
                for (const auto& entry : std::filesystem::directory_iterator(rutaDirectorioUsuario)) {
                    if (entry.is_regular_file()) {
                        listaArchivos.push_back(entry.path().filename().string());
                    }
                }
            }
            // Si el directorio no existe, simplemente devuelve la lista vacía.
        } catch (const std::filesystem::filesystem_error& e) {
             throw std::runtime_error("Error de filesystem al listar archivos para " + usuario.getNombreUsuario() + ": " + std::string(e.what()));
        }
    }
    return listaArchivos;
}