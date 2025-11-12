#ifndef GESTORUSUARIOS_H
#define GESTORUSUARIOS_H

#include <string>
#include <map>

using namespace std;

struct Usuario {
    string password;
    bool esAdmin;
};

class GestorUsuarios {
private:
    map<string, Usuario> mapaUsuarios;
    string archivoDeUsuarios;

    void cargarUsuariosDesdeArchivo();
    void guardarUsuariosEnArchivo();

public:
    GestorUsuarios(const string& archivo);
    ~GestorUsuarios();

    bool validarUsuario(const string& username, const string& password);
    bool esAdministrador(const string& username);
    bool crearUsuario(const string& username, const string& password, bool esAdmin);
    bool eliminarUsuario(const string& username);
    string listarUsuarios() const;
};

#endif