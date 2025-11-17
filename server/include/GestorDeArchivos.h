#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include <string>
#include "BaseDeDatos.h" 
#include <vector>

using namespace std;

class GestorDeArchivos {
private:
    string directorioBase;

    string obtenerRutaSegura(const string& nombreArchivo) const;

public:
    GestorDeArchivos(const string& dirBase = "server_logs");

    bool almacenarArchivo(const RegistroUsuarioDAO& usuario, const string& nombreArchivo, const string& contenido);

    bool validarAccesoArchivo(const RegistroUsuarioDAO& usuario, const string& nombreArchivo) const;

    string obtenerContenidoArchivo(const RegistroUsuarioDAO& usuario, const string& nombreArchivo) const;
    
    vector<string> listarArchivos() const;
};

#endif