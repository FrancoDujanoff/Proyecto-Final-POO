#ifndef CONTROLADORGENERAL_H
#define CONTROLADORGENERAL_H

#include <string>
#include <vector>

class ControladorRobot;
class GestorUsuarios;
class GestorDeArchivos;
class GestorDeReportes;
class PALogger;

// ControladorGeneral: orquestador principal del servidor.
// Aquí se declaran los métodos que usa el AdminCLI.
class ControladorGeneral {
public:
  ControladorGeneral();
  virtual ~ControladorGeneral();

  // Autenticación
  bool login(const std::string& usuario, const std::string& password);
  bool esAdministrador(const std::string& usuario) const;

  // Robot
  std::string solicitarReporteEstadoRobot();
  void alternarConexionRobot();
  void alternarAccesoRemoto();
  void alternarMotores();
  void irAHome();
  void moverRobot(float x, float y, float z, float velocidad);
  void alternarEfectorFinal();

  // Reportes
  std::string solicitarReporteAdmin();
  std::string solicitarReporteLog();

  // Shutdown
  void shutdownServidor();

private:
  // Implementación pendiente: punteros/instancias a subsistemas
  ControladorRobot* controladorRobot;
  GestorUsuarios* gestorUsuarios;
  GestorDeArchivos* gestorArchivos;
  GestorDeReportes* gestorReportes;
  PALogger* logger;
};

#endif // CONTROLADORGENERAL_H
