#ifndef MENSAJE_SERVIDOR_H
#define MENSAJE_SERVIDOR_H

#include <string>
#include <any>       // Para almacenar datos de tipo variable
#include <stdexcept> // Para excepciones estándar
#include <typeinfo>  // Para typeid

//Este es un enum que se crea con codigos de mensajes específicos para la comunicación
//entre el servidor y los clientes. Cada código representa un tipo particular de mensaje
//o una respuesta que el servidor puede enviar al cliente. 
//Falta especificar cuales van a ser exactamente, hay puestos ejemplos
enum class CodigoMensaje {
    // Peticiones del Cliente (Ejemplos)
    PETICION_LOGIN,
    PETICION_MOVER_ROBOT,
    PETICION_SUBIR_GCODE,
    PETICION_LISTAR_ARCHIVOS,
    // ... otros comandos ...

    // Respuestas del Servidor (Ejemplos)
    RESPUESTA_OK,
    RESPUESTA_ERROR_AUTENTICACION,
    RESPUESTA_ERROR_PERMISOS,
    RESPUESTA_ERROR_ROBOT_NO_CONECTADO,
    RESPUESTA_ERROR_ARCHIVO_NO_ENCONTRADO,
    RESPUESTA_ERROR_COMANDO_INVALIDO,
    RESPUESTA_DATOS_REPORTE,
    // ... otros estados/errores ...

    // Códigos genéricos
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