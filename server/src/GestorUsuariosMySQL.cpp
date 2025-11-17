#include "GestorUsuariosMySQL.h"
#include <stdexcept>
#include <iostream>

// --- Includes del Conector MySQL ---
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;

GestorUsuariosMySQL::GestorUsuariosMySQL(
    const string& host,
    const string& user,
    const string& pass,
    const string& schema)
    : db_host(host), db_user(user), db_pass(pass), db_schema(schema) 
{
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        conectar();
        cout << "INFO: Conexión a MySQL establecida con éxito." << endl;
    } catch (sql::SQLException &e) {
        cerr << "ERROR FATAL: No se pudo conectar a MySQL." << endl;
        throw runtime_error("Error al conectar a MySQL (" + string(e.what()) + ")");
    }
}

GestorUsuariosMySQL::~GestorUsuariosMySQL() {
    // La conexión se cierra automáticamente por unique_ptr
}

void GestorUsuariosMySQL::conectar() {
    con.reset(driver->connect(db_host, db_user, db_pass));
    con->setSchema(db_schema);
}

// --- IMPLEMENTACIÓN REAL (USUARIOS -> MySQL) ---

optional<RegistroUsuarioDAO> GestorUsuariosMySQL::buscarUsuarioPorNombre(const string& nombre) const {
    try {
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT id, nombre, clave_hash, es_admin FROM usuarios WHERE nombre = ?"
        ));
        pstmt->setString(1, nombre);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            RegistroUsuarioDAO usuario;
            usuario.id = res->getInt("id");
            usuario.nombre = res->getString("nombre");
            usuario.claveHasheada = res->getString("clave_hash");
            usuario.esAdmin = res->getBoolean("es_admin");
            return usuario;
        }
        return nullopt; 
    } catch (sql::SQLException &e) {
        throw runtime_error("Error SQL en buscarUsuarioPorNombre: " + string(e.what()));
    }
}

optional<RegistroUsuarioDAO> GestorUsuariosMySQL::buscarUsuarioPorId(int id) const {
    try {
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT id, nombre, clave_hash, es_admin FROM usuarios WHERE id = ?"
        ));
        pstmt->setInt(1, id);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            RegistroUsuarioDAO usuario;
            usuario.id = res->getInt("id");
            usuario.nombre = res->getString("nombre");
            usuario.claveHasheada = res->getString("clave_hash");
            usuario.esAdmin = res->getBoolean("es_admin");
            return usuario;
        }
        return nullopt;
    } catch (sql::SQLException &e) {
        throw runtime_error("Error SQL en buscarUsuarioPorId: " + string(e.what()));
    }
}

bool GestorUsuariosMySQL::crearUsuario(const RegistroUsuarioDAO& u) {
    if (u.claveHasheada.empty()) return false;
    
    try {
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "INSERT INTO usuarios(nombre, clave_hash, es_admin) VALUES(?, ?, ?)"
        ));
        pstmt->setString(1, u.nombre);
        pstmt->setString(2, u.claveHasheada);
        pstmt->setBoolean(3, u.esAdmin);

        pstmt->executeUpdate(); 
        return true; 

    } catch (sql::SQLException &e) {
        cerr << "Error SQL en crearUsuario: " << e.what() << endl;
        return false;
    }
}

bool GestorUsuariosMySQL::actualizarUsuario(const RegistroUsuarioDAO& u) {
    try {
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "UPDATE usuarios SET clave_hash = ?, es_admin = ? WHERE id = ?"
        ));
        pstmt->setString(1, u.claveHasheada);
        pstmt->setBoolean(2, u.esAdmin);
        pstmt->setInt(3, u.id);

        pstmt->executeUpdate();
        return true;
    } catch (sql::SQLException &e) {
        return false;
    }
}

bool GestorUsuariosMySQL::eliminarUsuarioPorNombre(const string& nombre) {
    try {
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "DELETE FROM usuarios WHERE nombre = ?"
        ));
        pstmt->setString(1, nombre);
        
        int rows_affected = pstmt->executeUpdate();
        return (rows_affected > 0); 

    } catch (sql::SQLException &e) {
        cerr << "Error SQL en eliminarUsuarioPorNombre: " << e.what() << endl;
        return false;
    }
}

std::vector<RegistroUsuarioDAO> GestorUsuariosMySQL::listarUsuarios() const {
    std::vector<RegistroUsuarioDAO> lista;
    try {
        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT id, nombre, clave_hash, es_admin FROM usuarios ORDER BY nombre"
        ));

        while (res->next()) {
            RegistroUsuarioDAO usuario;
            usuario.id = res->getInt("id");
            usuario.nombre = res->getString("nombre");
            usuario.claveHasheada = "[OCULTO]"; 
            usuario.esAdmin = res->getBoolean("es_admin");
            lista.push_back(usuario);
        }

    } catch (sql::SQLException &e) {
        throw runtime_error("Error SQL en listarUsuarios: " + string(e.what()));
    }
    return lista;
}

// --- MÉTODOS "DUMMY" (ACTIVIDAD) ---
// Se mantienen vacíos para usar el log de archivos en su lugar,
// pero deben existir para cumplir con la interfaz BaseDeDatos.

bool GestorUsuariosMySQL::registrarActividad(const RegistroActividadDAO& a) {
    (void)a; return true; // No hacemos nada en BD
}

vector<RegistroActividadDAO> GestorUsuariosMySQL::listarActividadPorUsuario(int idUsuario) const {
    (void)idUsuario; return {}; // Retornamos vacío
}

vector<RegistroActividadDAO> GestorUsuariosMySQL::listarActividadGlobal() const {
    return {}; // Retornamos vacío
}