#ifndef PALOGGER_H
#define PALOGGER_H

#include <string>
#include <vector>
#include <optional>
#include <chrono>

// Niveles de log utilizados por PALogger y GestorDeReportes
enum class NivelLog {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

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

    // Tipos y métodos para consultas de log/reportes
    struct LogEntry {
        NivelLog nivel;
        int idUsuario; // opcional: 0 si no aplica
        std::string timestampISO;
        std::string modulo;
        std::string mensaje;
        bool esError;
    };

    struct FiltroLog {
        std::optional<int> idUsuario;
        std::optional<NivelLog> minNivel;
        std::optional<std::string> contiene;
        std::optional<std::chrono::system_clock::time_point> desde;
        std::optional<std::chrono::system_clock::time_point> hasta;
    };

    // Consultar entradas de log que cumplan filtros (retorna copia)
    std::vector<LogEntry> consultar(const FiltroLog& filtro) const;

    // Formatear una entrada de log como string legible
    std::string formatear(const LogEntry& e) const;

private:
    std::string logFIle; // se respeta el nombre del UML

public:
    void setLogFIle(const std::string& value) { logFIle = value; }
    std::string getLogFIle() const { return logFIle; }

private:
    void initAttributes();

    // In-memory store minimal para desarrollo
    std::vector<LogEntry> store;
};

#endif // PALOGGER_H
