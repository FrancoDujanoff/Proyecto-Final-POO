#ifndef MENSAJE_SERVIDOR_H
#define MENSAJE_SERVIDOR_H

#include <string>
#include <any>       // Para almacenar datos de tipo variable
#include <stdexcept> // Para excepciones estándar
#include <typeinfo>  // Para typeid

enum class CodigoMensaje {
    //Peticiones del Cliente
    PETICION_LOGIN,
    PETICION_MOVER_ROBOT,
    PETICION_SUBIR_GCODE,
    PETICION_LISTAR_ARCHIVOS,

    // Respuestas del Servidor 
    RESPUESTA_OK,
    RESPUESTA_ERROR_AUTENTICACION,
    RESPUESTA_ERROR_PERMISOS,
    RESPUESTA_ERROR_ROBOT_NO_CONECTADO,
    RESPUESTA_ERROR_ARCHIVO_NO_ENCONTRADO,
    RESPUESTA_ERROR_COMANDO_INVALIDO,
    RESPUESTA_DATOS_REPORTE,

    ERROR_DESCONOCIDO = -1
};


class MensajeServidor {
private:
    CodigoMensaje codigo;
    std::string mensaje;
    std::any dato;

public:

    //Constructor
    MensajeServidor(CodigoMensaje cod = CodigoMensaje::RESPUESTA_OK,
                    const std::string& msg = "",
                    const std::any& payload = std::any());


    CodigoMensaje getCodigo() const;

    void setCodigo(CodigoMensaje cod);

    const std::string& getMensaje() const;

    void setMensaje(const std::string& msg);

    const std::any& getDato() const;

    void setDato(const std::any& payload);


    ~MensajeServidor() = default;
};

#endif // MENSAJE_SERVIDOR_H