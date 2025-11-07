#ifndef PALOGGER_H
#define PALOGGER_H

#include <string>
#include <vector>

class PALogger {
public:
    PALogger();
    virtual ~PALogger();

    // Niveles de log básicos
    void info(const std::string& modulo, const std::string& mensaje);
    void warning(const std::string& modulo, const std::string& mensaje);

    // Devuelve la ruta del archivo de log (si se usa archivo)
    std::string getLogFilePath() const;

    // Eventos generales
    void logEvento(const std::string& modulo, const std::string& mensaje);

    // Traza de petición/respuesta
    void logPeticion(const std::string& timestamp, const std::string& usuario, const std::string& nodo, const std::string& peticion, const std::string& respuesta);

private:
    std::string logFIle; // se respeta el nombre del UML

public:
    void setLogFIle(const std::string& value) { logFIle = value; }
    std::string getLogFIle() const { return logFIle; }

private:
    void initAttributes();
};

#endif // PALOGGER_H
