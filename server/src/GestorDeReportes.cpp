#include <sstream>
#include <chrono>
#include <iomanip>

#include "GestorDeReportes.h"
#include "PALogger.h"
#include "BaseDeDatos.h"
#include "UsuarioServidor.h"

static std::optional<std::chrono::system_clock::time_point> parseISO8601(const std::optional<std::string>& s) {
    if (!s.has_value()) return std::nullopt;
    std::tm tm{};




    std::istringstream iss(*s);
    if (s->size() == 10)
        iss >> std::get_time(&tm, "%Y-%m-%d");
    else
        iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (!iss.fail()) {
        auto tt = std::mktime(&tm);
        if (tt != -1) return std::chrono::system_clock::from_time_t(tt);
    }
    return std::nullopt;
}

GestorDeReportes::GestorDeReportes(PALogger& l, BaseDeDatos& b) : logger(l), bd(b) {}

std::vector<std::string> GestorDeReportes::generarReporteActividad(const UsuarioServidor& usuario) {
    std::vector<std::string> out;
    auto actividades = bd.listarActividadPorUsuario(usuario.getId());
    out.reserve(actividades.size() + 2);
    out.push_back("=== REPORTE DE ACTIVIDAD (Usuario: " + std::to_string(usuario.getId()) + ") ===");
    for (const auto& a : actividades) {
        out.push_back(a.fechaHoraISO8601 + " | " + (a.esError ? "ERROR" : "OK   ") + " | " + a.descripcion);
    }
    if (actividades.empty()) out.push_back("(sin actividad registrada)");
    return out;
}

std::vector<std::string> GestorDeReportes::generarReporteAdmin(const UsuarioServidor& admin, const FiltrosReporteAdmin& filtros) {
    std::vector<std::string> out;
    if (!admin.isAdmin()) {
        out.push_back("[PERMISO DENEGADO] Sólo un administrador puede generar este reporte.");
        return out;
    }
    auto todas = bd.listarActividadGlobal();
    out.push_back("=== REPORTE ADMINISTRADOR: ACTIVIDAD ===");
    for (const auto& a : todas) {
        if (filtros.idUsuario && a.idUsuario != *filtros.idUsuario) continue;
        if (filtros.contiene && a.descripcion.find(*filtros.contiene) == std::string::npos) continue;
        out.push_back("usr=" + std::to_string(a.idUsuario) + " | " + a.fechaHoraISO8601 + (a.esError ? " | ERROR | " : " | OK    | ") + a.descripcion);
    }
    if (out.size() == 1) out.push_back("(sin resultados con esos filtros)");
    return out;
}

std::vector<std::string> GestorDeReportes::generarReporteLog(const UsuarioServidor& admin, const FiltrosReporteAdmin& filtros) {
    std::vector<std::string> out;
    if (!admin.isAdmin()) {
        out.push_back("[PERMISO DENEGADO] Sólo un administrador puede ver el log.");
        return out;
    }
    PALogger::FiltroLog fLog;
    fLog.idUsuario = filtros.idUsuario;
    fLog.minNivel = filtros.minNivelLog;
    fLog.contiene = filtros.contiene;
    fLog.desde = parseISO8601(filtros.desdeISO);
    fLog.hasta = parseISO8601(filtros.hastaISO);

    auto entradas = logger.consultar(fLog);
    out.push_back("=== REPORTE ADMIN: LOG DEL SERVIDOR ===");
    for (const auto& e : entradas) {
        out.push_back(logger.formatear(e));
    }
    if (entradas.empty()) out.push_back("(sin entradas de log con esos filtros)");
    return out;
}
