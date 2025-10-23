#include "MensajeServidor.h"
#include <string>    //Incluido por el .h, pero buena práctica repetirlo si se usa directamente
#include <any>       //Incluido por el .h


MensajeServidor::MensajeServidor(CodigoMensaje cod, const std::string& msg, const std::any& payload)
    : codigo(cod), mensaje(msg), dato(payload)
{
    
}


CodigoMensaje MensajeServidor::getCodigo() const {

    return codigo;
}

void MensajeServidor::setCodigo(CodigoMensaje cod) {

    codigo = cod;
}

const std::string& MensajeServidor::getMensaje() const {

    //Devuelve por referencia constante para eficiencia.
    return mensaje;
}

void MensajeServidor::setMensaje(const std::string& msg) {

    mensaje = msg;
}

const std::any& MensajeServidor::getDato() const {

    //Devuelve el std::any por referencia constante.
    return dato;
}

void MensajeServidor::setDato(const std::any& payload) {

    dato = payload;
}

