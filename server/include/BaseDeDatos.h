#ifndef BASE_DE_DATOS_H
#define BASE_DE_DATOS_H


#include <string>
#include <vector>
#include <optional>


class UsuarioServidor; // forward


struct RegistroUsuarioDAO {
int id;
std::string nombre;
std::string claveHasheada; // ver notas de seguridad
bool esAdmin;
};


struct RegistroActividadDAO {
int id;
int idUsuario;
std::string descripcion; // resumen de acción
std::string fechaHoraISO8601; // ISO 8601 para portabilidad
bool esError;
};


// Interfaz de BD para cumplir la consigna (persistir usuarios y actividad)
class BaseDeDatos {
public:
virtual ~BaseDeDatos() = default;


// Usuarios
virtual std::optional<RegistroUsuarioDAO> buscarUsuarioPorNombre(const std::string& nombre) const = 0;
virtual std::optional<RegistroUsuarioDAO> buscarUsuarioPorId(int id) const = 0;
virtual bool crearUsuario(const RegistroUsuarioDAO& u) = 0;
virtual bool actualizarUsuario(const RegistroUsuarioDAO& u) = 0;


// Actividad
virtual bool registrarActividad(const RegistroActividadDAO& a) = 0;
virtual std::vector<RegistroActividadDAO> listarActividadPorUsuario(int idUsuario) const = 0;
virtual std::vector<RegistroActividadDAO> listarActividadGlobal() const = 0;


// No copiable por defecto
BaseDeDatos() = default;
BaseDeDatos(const BaseDeDatos&) = delete;
BaseDeDatos& operator=(const BaseDeDatos&) = delete;
};


#endif // BASE_DE_DATOS_H