#ifndef GESTOR_USUARIOS_MYSQL_H
#define GESTOR_USUARIOS_MYSQL_H

#include "BaseDeDatos.h" // ¡Heredamos de la interfaz existente!
#include <string>
#include <memory>       // Para std::unique_ptr

// --- Declaraciones adelantadas del Conector MySQL ---
// (Evitamos incluir los headers pesados de MySQL aquí)
namespace sql {
    class Connection;
    class Driver;
}

/**
 * @class GestorUsuariosMySQL
 * @brief Implementación CONCRETA de la interfaz BaseDeDatos.
 * Gestiona la persistencia de usuarios y logs de actividad 
 * usando un servidor MySQL real.
 */
class GestorUsuariosMySQL : public BaseDeDatos {
private:
    // --- Miembros del Conector MySQL ---
    sql::Driver* driver;
    std::unique_ptr<sql::Connection> con; // unique_ptr para gestión RAII

    // --- Detalles de Conexión ---
    std::string db_host;
    std::string db_user;
    std::string db_pass;
    std::string db_schema;

    /**
     * @brief Método privado para establecer o reestablecer la conexión
     */
    void conectar();

public:
    /**
     * @brief Constructor que establece la conexión.
     */
    GestorUsuariosMySQL(
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& schema
    );

    /**
     * @brief Destructor que cierra la conexión.
     */
    virtual ~GestorUsuariosMySQL();

    // --- Implementación de la Interfaz BaseDeDatos ---
    // (Note el uso de 'override' para asegurar la correctitud polimórfica)

    // Usuarios
    std::optional<RegistroUsuarioDAO> buscarUsuarioPorNombre(const std::string& nombre) const override;
    std::optional<RegistroUsuarioDAO> buscarUsuarioPorId(int id) const override;
    bool crearUsuario(const RegistroUsuarioDAO& u) override;
    bool actualizarUsuario(const RegistroUsuarioDAO& u) override;

    bool eliminarUsuarioPorNombre(const std::string& nombre) override;

    std::vector<RegistroUsuarioDAO> listarUsuarios() const override;

    // Actividad
    bool registrarActividad(const RegistroActividadDAO& a) override;
    std::vector<RegistroActividadDAO> listarActividadPorUsuario(int idUsuario) const override;
    std::vector<RegistroActividadDAO> listarActividadGlobal() const override;

    // Evitamos copias
    GestorUsuariosMySQL(const GestorUsuariosMySQL&) = delete;
    GestorUsuariosMySQL& operator=(const GestorUsuariosMySQL&) = delete;
};

#endif // GESTOR_USUARIOS_MYSQL_H