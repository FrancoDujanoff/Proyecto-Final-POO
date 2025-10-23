#ifndef MENSAJE_SERVIDOR_H
#define MENSAJE_SERVIDOR_H

#include <string>
#include <any> 

//Codigos del Mensaje, usamos un emum class para mayor seguridad y claridad.
//Esto por ahora queda asi pero mas adelante tenemos que ver bien que Codigos de Mensaje definir

enum class CodigoMensaje {
    //Peticiones de Cliente
    PETICION_LOGIN,
    PETICION_MOVER_ROBOT,
    PETICION_SUBIR_GCODE,
    PETICION_LISTAR_ARCHIVOS,
    // ... otros comandos ...

    //Respuestas del Servidor
    RESPUESTA_OK,
    RESPUESTA_ERROR_AUTENTICACION,
    RESPUESTA_ERROR_PERMISOS,
    RESPUESTA_ERROR_ROBOT_NO_CONECTADO,
    RESPUESTA_ERROR_ARCHIVO_NO_ENCONTRADO,
    RESPUESTA_ERROR_COMANDO_INVALIDO,
    RESPUESTA_DATOS_REPORTE,
    // ... otros estados/errores ...

    //Codigos genericos
    ERROR_DESCONOCIDO = -1
};

class MensajeServidor {
private:
    
    CodigoMensaje codigo;   //Código que identifica el tipo/estado del mensaje
    std::string mensaje;    //Mensaje textual descriptivo
    std::any dato;          //Contenedor para los datos adicionales 

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

    template <typename T>
    T getDatoComo() const {
        try {
            return std::any_cast<T>(dato);
        } catch (const std::bad_any_cast& e) {
            // Manejar el error: el tipo solicitado no es el almacenado.
            // Podríamos lanzar una excepción personalizada, retornar un valor por defecto,
            // o registrar un error. Lanzar es a menudo lo más claro.
            throw std::runtime_error("Error al obtener dato: Tipo incorrecto solicitado. " + std::string(e.what()));
        }
    }

     // Método de conveniencia para verificar el tipo almacenado en 'dato'
    template <typename T>
    bool datoEsTipo() const {
        return dato.type() == typeid(T);
    }


    // --- Métodos de Serialización/Deserialización (Marcadores de posición) ---
    // La implementación exacta dependerá de cómo la biblioteca XML-RPC
    // espera/entrega los datos. Podrían no ser necesarios si la biblioteca
    // maneja la conversión directamente desde/hacia estructuras o mapas.

    // std::string serializar() const; // Convierte el objeto a string (ej: XML, JSON)
    // static MensajeServidor deserializar(const std::string& data); // Crea objeto desde string

   
    ~MensajeServidor() = default; // El destructor por defecto es suficiente
};

#endif // MENSAJE_SERVIDOR_H