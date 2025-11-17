#include "GestorDeArchivos.h"
#include <fstream>   // Para leer/escribir (ifstream, ofstream)
#include <sstream>   // Para leer archivos a string (stringstream)
#include <iostream>  // Para logs de error
#include <sys/stat.h> // Para mkdir
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

GestorDeArchivos::GestorDeArchivos(const string& dirBase) 
    : directorioBase(dirBase) 
{
    // Intentamos crear el directorio base si no existe
    // (Aviso: esto es específico de Linux/macOS, para Windows usa _mkdir)
    mkdir(directorioBase.c_str(), 0755); 
}

/**
 * Método privado de seguridad.
 * Previene que alguien pida un archivo como "../passwd".
 */
string GestorDeArchivos::obtenerRutaSegura(const string& nombreArchivo) const {
    // Comprobación de seguridad básica: prohibir ".." (subir de directorio)
    if (nombreArchivo.empty() || nombreArchivo.find("..") != string::npos) {
        cerr << "ERROR (Seguridad): Intento de acceso a ruta invalida: " << nombreArchivo << endl;
        return ""; // Ruta no segura
    }
    
    // Devuelve la ruta completa (ej. "server_logs/mi_log.txt")
    return directorioBase + "/" + nombreArchivo;
}

bool GestorDeArchivos::almacenarArchivo(const RegistroUsuarioDAO& usuario, const string& nombreArchivo, const string& contenido) {
    (void)usuario; // Ignoramos el parámetro usuario como decidimos

    string rutaSegura = obtenerRutaSegura(nombreArchivo);
    if (rutaSegura.empty()) {
        return false;
    }

    // Usamos 'ofstream' (output file stream)
    // ios::app = Modo "Append" (añadir al final), si quisiéramos sobreescribir usaríamos ios::trunc
    ofstream archivo(rutaSegura, ios::app); 
    
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir '" << rutaSegura << "' para escribir." << endl;
        return false;
    }

    archivo << contenido; // Escribimos el contenido
    archivo.close();
    return true;
}

bool GestorDeArchivos::validarAccesoArchivo(const RegistroUsuarioDAO& usuario, const string& nombreArchivo) const {
    (void)usuario; // Ignoramos el parámetro usuario

    string rutaSegura = obtenerRutaSegura(nombreArchivo);
    if (rutaSegura.empty()) {
        return false;
    }

    // Usamos 'ifstream' (input file stream)
    ifstream archivo(rutaSegura);
    return archivo.good(); // Devuelve true si el archivo existe y se puede leer
}

string GestorDeArchivos::obtenerContenidoArchivo(const RegistroUsuarioDAO& usuario, const string& nombreArchivo) const {
    (void)usuario; // Ignoramos el parámetro usuario

    string rutaSegura = obtenerRutaSegura(nombreArchivo);
    if (rutaSegura.empty()) {
        return "ERROR: Acceso de archivo no seguro.";
    }

    ifstream archivo(rutaSegura);
    if (!archivo.is_open()) {
        return "ERROR: No se pudo leer el archivo.";
    }

    // Este es un truco estándar de C++ para leer un archivo entero en un string
    stringstream buffer;
    buffer << archivo.rdbuf();
    archivo.close();
    
    return buffer.str();
}


vector<string> GestorDeArchivos::listarArchivos() const {
    vector<string> lista;
    try {
        // Iterar sobre el directorio
        for (const auto& entry : fs::directory_iterator(directorioBase)) {
            if (entry.is_regular_file()) {
                lista.push_back(entry.path().filename().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "ERROR al listar archivos: " << e.what() << endl;
    }
    return lista;
}