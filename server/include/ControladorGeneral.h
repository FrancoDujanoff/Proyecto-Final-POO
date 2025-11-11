/*
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
*/

#ifndef CONTROLADORGENERAL_H
#define CONTROLADORGENERAL_H

#include <string>
#include <vector>
#include <memory> // <--- NECESARIO PARA unique_ptr
#include "ControladorRobot.h" // <--- NECESARIO PARA unique_ptr

// Gestores que no usaremos en esta prueba (solo declaraciones)
class GestorUsuarios;
class GestorDeArchivos;
class GestorDeReportes;
class PALogger;

class ControladorGeneral {
public:
    // Constructor ahora acepta los detalles de conexión
    ControladorGeneral(const std::string& puerto, int baudrate);
    virtual ~ControladorGeneral();

    // --- MÉTODOS DE AUTENTICACIÓN (SIMULADOS) ---
    bool login(const std::string& usuario, const std::string& password);
    bool esAdministrador(const std::string& usuario) const;

    // --- MÉTODOS DEL ROBOT (DELEGADOS) ---
    std::string solicitarReporteEstadoRobot();
    void alternarConexionRobot();
    void alternarMotores();
    void irAHome();
    void moverRobot(float x, float y, float z, float velocidad);
    void alternarEfectorFinal();

    // --- NUEVOS MÉTODOS DELEGADOS (DE TU ADMINCLI.CPP) ---
    bool pausar(float segundos);
    bool setModoCoordenadas(bool absoluto);
    bool definirPosicionActual(float x, float y, float z, float e = NAN);
    bool controlarVentilador(bool estado);
    std::string solicitarReporteEndstops();

    // --- MÉTODOS VACÍOS (NO REQUERIDOS PARA ESTA PRUEBA) ---
    void alternarAccesoRemoto();
    std::string solicitarReporteAdmin();
    std::string solicitarReporteLog();
    void shutdownServidor();

private:
    // ¡Usamos un puntero inteligente!
    std::unique_ptr<ControladorRobot> controladorRobot; 
    
    // Dejamos los otros como punteros nulos
    GestorUsuarios* gestorUsuarios;
    GestorDeArchivos* gestorArchivos;
    GestorDeReportes* gestorReportes;
    PALogger* logger;
};

#endif // CONTROLADORGENERAL_H
