#include "PALogger.h"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>

// Constructors/Destructors


PALogger::PALogger()
{
  initAttributes();
}

PALogger::~PALogger()
{
}

// Methods


// Accessor methods



// Other methods


void PALogger::initAttributes()
{
}

void PALogger::info(const std::string& modulo, const std::string& mensaje) {
  LogEntry e;
  e.nivel = NivelLog::INFO;
  e.idUsuario = 0;
  e.timestampISO = ""; // se puede llenar con hora actual si se requiere
  e.modulo = modulo;
  e.mensaje = mensaje;
  e.esError = false;
  store.push_back(e);
}

void PALogger::warning(const std::string& modulo, const std::string& mensaje) {
  LogEntry e;
  e.nivel = NivelLog::WARNING;
  e.idUsuario = 0;
  e.timestampISO = "";
  e.modulo = modulo;
  e.mensaje = mensaje;
  e.esError = true;
  store.push_back(e);
}

void PALogger::logEvento(const std::string& modulo, const std::string& mensaje) {
  info(modulo, mensaje);
}

void PALogger::logPeticion(const std::string& timestamp, const std::string& usuario, const std::string& nodo, const std::string& peticion, const std::string& respuesta) {
  LogEntry e;
  e.nivel = NivelLog::INFO;
  e.idUsuario = 0;
  e.timestampISO = timestamp;
  e.modulo = nodo + "::" + usuario;
  e.mensaje = peticion + " -> " + respuesta;
  e.esError = false;
  store.push_back(e);
}

std::vector<PALogger::LogEntry> PALogger::consultar(const FiltroLog& filtro) const {
  std::vector<LogEntry> out;
  for (const auto& e : store) {
    if (filtro.idUsuario && e.idUsuario != *filtro.idUsuario) continue;
    if (filtro.minNivel && static_cast<int>(e.nivel) < static_cast<int>(*filtro.minNivel)) continue;
    if (filtro.contiene && e.mensaje.find(*filtro.contiene) == std::string::npos) continue;
    // filtros de tiempo ignorados por simplicidad
    out.push_back(e);
  }
  return out;
}

std::string PALogger::formatear(const PALogger::LogEntry& e) const {
  std::ostringstream ss;
  ss << "[" << (e.timestampISO.empty() ? "now" : e.timestampISO) << "] ";
  switch (e.nivel) {
    case NivelLog::DEBUG: ss << "DEBUG"; break;
    case NivelLog::INFO: ss << "INFO"; break;
    case NivelLog::WARNING: ss << "WARN"; break;
    case NivelLog::ERROR: ss << "ERROR"; break;
  }
  ss << " " << e.modulo << " - " << e.mensaje;
  return ss.str();
}

