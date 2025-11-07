#ifndef GESTOR_DE_REPORTES_H
#define GESTOR_DE_REPORTES_H

#include <string>
#include <vector>
#include <utility>
#include <optional>

class PALogger;
class BaseDeDatos;
class UsuarioServidor;

// NOTA: NivelLog no está definido en los archivos que me diste.
// Posiblemente falte un enum en PALogger.h o un archivo común.
struct FiltrosReporteAdmin {
    std::optional<int> idUsuario;
    std::optional<NivelLog> minNivelLog;
    std::optional<std::string> contiene;
    std::optional<std::string> desdeISO;
    std::optional<std::string> hastaISO;
};

class GestorDeReportes {
private:
    PALogger& logger;
    BaseDeDatos& bd;

public:
    explicit GestorDeReportes(PALogger& l, BaseDeDatos& b);

    std::vector<std::string> generarReporteActividad(const UsuarioServidor& usuario);

    std::vector<std::string> generarReporteAdmin(const UsuarioServidor& admin, const FiltrosReporteAdmin& filtros);

    std::vector<std::string> generarReporteLog(const UsuarioServidor& admin, const FiltrosReporteAdmin& filtros);

    GestorDeReportes(const GestorDeReportes&) = delete;
    GestorDeReportes& operator=(const GestorDeReportes&) = delete;
};

#endif // GESTOR_DE_REPORTES_H
