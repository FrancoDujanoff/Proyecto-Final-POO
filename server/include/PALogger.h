#ifndef PALOGGER_H
#define PALOGGER_H

#include <string>
#include "GestorDeArchivos.h"
#include "BaseDeDatos.h" 

using namespace std;

class PALogger {
private:
    string logFile; 
    GestorDeArchivos* gestorDeArchivos;

    RegistroUsuarioDAO usuarioSistema; 

    void log(const string& nivel, const string& mensaje);

public:
    PALogger(GestorDeArchivos* gestor, const string& archivoLog = "servidor.log");

    void info(const string& mensaje);
    void warning(const string& mensaje);
    void logEvento(const string& modulo, const string& mensaje);
    void logPeticion(const string& timestamp, const string& usuario, 
                   const string& nodo, const string& peticion, 
                   const string& respuesta);

    string getLogFilePath() const;
};

#endif // PALOGGER_H