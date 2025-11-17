#include "ServidorRPC.h"
#include "ControladorGeneral.h" // Necesitamos la definición completa

// --- Includes de la Biblioteca XML-RPC ---
// Ahora sí incluimos las definiciones completas
#include "XmlRpcServer.h"
#include "XmlRpcServerMethod.h"
#include "XmlRpcValue.h"
#include "XmlRpcException.h"

// --- Includes Estándar ---
#include <iostream>
#include <string>
#include <vector>

// Usamos el namespace de la biblioteca
using namespace XmlRpc;

/*
======================================================================
  ESPACIO DE NOMBRES ANÓNIMO: CLASES ADAPTADORAS (PRIVADAS)
======================================================================
 (Estas clases solo son visibles dentro de ESTE archivo .cpp)
*/
namespace {

// --- FUNCIÓN AUXILIAR PARA NÚMEROS (CORREGIDA) ---
// Debe estar aquí, dentro del namespace, donde XmlRpc::XmlRpcValue ya es visible
double obtenerNumero(XmlRpcValue& valor) {
    if (valor.getType() == XmlRpcValue::TypeDouble) {
        return (double)valor;
    } else if (valor.getType() == XmlRpcValue::TypeInt) {
        return (double)((int)valor); // Convierte int a double
    } else {
        throw XmlRpcException("Error de tipo: Se esperaba un numero (int o double).");
    }
}

/**
 * @class MetodoControlado
 * (Clase base, sin cambios respecto a la versión anterior funcional)
 */
class MetodoControlado : public XmlRpcServerMethod {
protected:
    ControladorGeneral* controlador;
    bool soloAdmin;

public:
    MetodoControlado(const std::string& name, XmlRpcServer* server, ControladorGeneral* c, bool adminOnly)
        : XmlRpcServerMethod(name, server), controlador(c), soloAdmin(adminOnly) {}

    void execute(XmlRpcValue& params, XmlRpcValue& result) override {
        try {
            if (!controlador->estaAccesoRemotoHabilitado()) {
                throw XmlRpcException("Acceso remoto deshabilitado por el administrador.", 100);
            }
            if (params.size() < 2) {
                throw XmlRpcException("Petición inválida. Se requieren al menos usuario y clave.", 101);
            }
            std::string usuario = params[0];
            std::string clave = params[1];

            if (!controlador->login(usuario, clave)) {
                throw XmlRpcException("Autenticación fallida. Usuario o clave incorrectos.", 102);
            }

            if (soloAdmin && !controlador->esAdministrador(usuario)) {
                throw XmlRpcException("Permiso denegado. Se requieren privilegios de administrador.", 103);
            }

            executeControlado(usuario, params, result);

        } catch (const XmlRpcException& e) {
            throw;
        } catch (const std::exception& e) {
            throw XmlRpcException("Error interno del servidor: " + std::string(e.what()), 500);
        } catch (...) {
            throw XmlRpcException("Error fatal desconocido en el servidor.", 501);
        }
    }

    virtual void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) = 0;
};

// --- ADAPTADORES BÁSICOS (Ya funcionaban) ---

class MetodoLogin : public XmlRpcServerMethod {
    ControladorGeneral* controlador;
public:
    MetodoLogin(XmlRpcServer* s, ControladorGeneral* c) 
        : XmlRpcServerMethod("robot.login", s), controlador(c) {}

    void execute(XmlRpcValue& params, XmlRpcValue& result) override {
        if (!controlador->estaAccesoRemotoHabilitado()) throw XmlRpcException("Acceso remoto deshabilitado.", 100);
        if (params.size() != 2) throw XmlRpcException("robot.login requiere 2 parámetros.");
        result = controlador->login(params[0], params[1]);
    }
};

class MetodoMoverRobot : public MetodoControlado {
public:
    MetodoMoverRobot(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.mover", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() < 5) throw XmlRpcException("robot.mover requiere 5 parámetros.");
        
        // USO DE obtenerNumero (CORREGIDO)
        float x = (float)obtenerNumero(params[2]);
        float y = (float)obtenerNumero(params[3]);
        float z = (float)obtenerNumero(params[4]);
        
        float vel = 10.0f;
        if (params.size() >= 6) {
            vel = (float)obtenerNumero(params[5]);
        }

        result = controlador->moverRobot(x, y, z, vel);
    }
};

class MetodoControlarMotores : public MetodoControlado {
public:
    MetodoControlarMotores(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarMotores", s, c, true) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("Requiere 3 params.");
        bool estado = params[2];
        result = controlador->controlarMotores(estado);
    }
};

class MetodoControlarConexion : public MetodoControlado {
public:
    MetodoControlarConexion(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarConexion", s, c, true) {} 

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("Requiere 3 params.");
        bool estado = params[2];
        result = controlador->controlarConexion(estado);
    }
};

class MetodoHome : public MetodoControlado {
public:
    MetodoHome(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.irAHome", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        result = controlador->irAHome();
    }
};

class MetodoControlarEfector : public MetodoControlado {
public:
    MetodoControlarEfector(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarEfector", s, c, false) {} 

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("Requiere 3 params.");
        bool estado = params[2];
        result = controlador->controlarEfector(estado);
    }
};

class MetodoReporteEstado : public MetodoControlado {
public:
    MetodoReporteEstado(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.solicitarReporteEstado", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        result = controlador->solicitarReporteEstadoRobot();
    }
};

// --- ADAPTADORES DE ARCHIVOS (Ya funcionaban) ---

class MetodoSubirArchivo : public MetodoControlado {
public:
    MetodoSubirArchivo(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.subirArchivo", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 4) throw XmlRpcException("Requiere nombre y contenido.");
        std::string nombre = params[2];
        std::string contenido = params[3];
        result = controlador->guardarArchivoGCode(nombre, contenido);
    }
};

class MetodoListarArchivos : public MetodoControlado {
public:
    MetodoListarArchivos(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.listarArchivos", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        result = controlador->listarArchivosGCode();
    }
};

class MetodoEjecutarArchivo : public MetodoControlado {
public:
    MetodoEjecutarArchivo(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.ejecutarArchivo", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("Requiere nombre archivo.");
        std::string nombre = params[2];
        result = controlador->ejecutarArchivoGCode(nombre);
    }
};

// --- ADAPTADORES DE APRENDIZAJE (Ya funcionaban) ---

class MetodoIniciarAprendizaje : public MetodoControlado {
public:
    MetodoIniciarAprendizaje(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.iniciarAprendizaje", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        result = controlador->iniciarAprendizaje();
    }
};

class MetodoRegistrarPunto : public MetodoControlado {
public:
    MetodoRegistrarPunto(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.registrarPunto", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        result = controlador->registrarPuntoActual();
    }
};

class MetodoGuardarTrayectoria : public MetodoControlado {
public:
    MetodoGuardarTrayectoria(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.guardarTrayectoria", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("Requiere nombre archivo.");
        std::string nombre = params[2];
        result = controlador->finalizarAprendizaje(nombre);
    }
};


// --- ADAPTADORES AVANZADOS (CORREGIDOS) ---

class MetodoPausar : public MetodoControlado {
public:
    MetodoPausar(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.pausar", s, c, false) {}

    // CORREGIDO: Firma completa y uso de obtenerNumero
    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("robot.pausar requiere: user, pass, segundos.");
        double seg = obtenerNumero(params[2]);
        result = controlador->pausar((float)seg);
    }
};

class MetodoModoCoordenadas : public MetodoControlado {
public:
    MetodoModoCoordenadas(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.setModoCoordenadas", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("robot.setModoCoordenadas requiere: user, pass, bool_absoluto.");
        bool absoluto = params[2];
        result = controlador->setModoCoordenadas(absoluto);
    }
};

class MetodoDefinirPosicion : public MetodoControlado {
public:
    MetodoDefinirPosicion(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.definirPosicionActual", s, c, true) {} 

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() < 6) throw XmlRpcException("robot.definirPosicionActual requiere: user, pass, x, y, z, e.");
        
        // USO DE obtenerNumero
        float x = (float)obtenerNumero(params[2]);
        float y = (float)obtenerNumero(params[3]);
        float z = (float)obtenerNumero(params[4]);
        float e = (float)obtenerNumero(params[5]);

        result = controlador->definirPosicionActual(x, y, z, e);
    }
};

class MetodoControlarVentilador : public MetodoControlado {
public:
    MetodoControlarVentilador(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarVentilador", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("robot.controlarVentilador requiere: user, pass, estado.");
        bool estado = params[2];
        result = controlador->controlarVentilador(estado);
    }
};

class MetodoReporteEndstops : public MetodoControlado {
public:
    MetodoReporteEndstops(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.solicitarReporteEndstops", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        result = controlador->solicitarReporteEndstops();
    }
};

} // Fin del namespace anónimo

/*
======================================================================
  IMPLEMENTACIÓN DE LA CLASE PÚBLICA ServidorRPC
======================================================================
*/

ServidorRPC::ServidorRPC(ControladorGeneral* controlador) 
    : refControladorGeneral(controlador), 
      servidorXmlRpc(std::make_unique<XmlRpcServer>()), // Crea la instancia del servidor
      estaCorriendo(false)
{
    if (refControladorGeneral == nullptr) {
        throw std::runtime_error("ServidorRPC no puede iniciar sin un ControladorGeneral.");
    }
    std::cout << "INFO: ServidorRPC creado. Listo para iniciar." << std::endl;
}

ServidorRPC::~ServidorRPC() {
    apagar(); // Nos aseguramos de que el hilo y el servidor se apaguen
}

void ServidorRPC::registrarMetodos() {
    std::cout << "INFO: Registrando métodos XML-RPC..." << std::endl;

    // --- Métodos de Autenticación y Administración ---
    servidorXmlRpc->addMethod(new MetodoLogin(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoControlarConexion(servidorXmlRpc.get(), refControladorGeneral));

    // --- Métodos de Control (Admin y Usuario) ---
    servidorXmlRpc->addMethod(new MetodoControlarMotores(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoHome(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoMoverRobot(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoControlarEfector(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoReporteEstado(servidorXmlRpc.get(), refControladorGeneral));
    
    // (AQUÍ SE REGISTRARÍAN LOS OTROS MÉTODOS CREADOS...)
    servidorXmlRpc->addMethod(new MetodoSubirArchivo(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoListarArchivos(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoEjecutarArchivo(servidorXmlRpc.get(), refControladorGeneral));
    
    servidorXmlRpc->addMethod(new MetodoIniciarAprendizaje(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoRegistrarPunto(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoGuardarTrayectoria(servidorXmlRpc.get(), refControladorGeneral));

    servidorXmlRpc->addMethod(new MetodoPausar(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoModoCoordenadas(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoDefinirPosicion(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoControlarVentilador(servidorXmlRpc.get(), refControladorGeneral));
    servidorXmlRpc->addMethod(new MetodoReporteEndstops(servidorXmlRpc.get(), refControladorGeneral));

    std::cout << "INFO: Métodos XML-RPC registrados." << std::endl;
}


void ServidorRPC::bucleDeServicio(int puerto) {
    try {
        // 1. Registra los métodos que definimos
        registrarMetodos();

        // 2. Vincula el servidor al puerto especificado
        std::cout << "INFO: Servidor XML-RPC iniciando en el puerto " << puerto << "..." << std::endl;
        servidorXmlRpc->bindAndListen(puerto);

        // 3. Inicia el bucle de trabajo
        // El bucle se ejecutará mientras 'estaCorriendo' sea true
        while (estaCorriendo) {
            // work(1.0) procesa peticiones pendientes durante 1 segundo
            // y luego devuelve el control, permitiendo que el bucle
            // verifique la bandera 'estaCorriendo'.
            servidorXmlRpc->work(1.0);
        }

        // 4. Inicia el apagado
        std::cout << "INFO: Bucle de servicio XML-RPC detenido." << std::endl;
        servidorXmlRpc->shutdown();

    } catch (const std::exception& e) {
        std::cerr << "ERROR FATAL EN HILO SERVIDOR RPC: " << e.what() << std::endl;
        estaCorriendo = false;
        // (En un sistema real, esto debería notificar al hilo principal)
    }
}

bool ServidorRPC::iniciar(int puerto) {
    if (estaCorriendo) {
        std::cerr << "WARN: ServidorRPC ya está corriendo." << std::endl;
        return false;
    }

    try {
        estaCorriendo = true;
        // Lanzamos el método 'bucleDeServicio' en un hilo separado
        // Le pasamos 'this' (el puntero a este objeto) y 'puerto' como argumentos
        hiloServidor = std::make_unique<std::thread>(&ServidorRPC::bucleDeServicio, this, puerto);
        std::cout << "INFO: Hilo de ServidorRPC lanzado." << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: No se pudo lanzar el hilo del servidor RPC: " << e.what() << std::endl;
        estaCorriendo = false;
        return false;
    }
}

void ServidorRPC::apagar() {
    if (!estaCorriendo) {
        return;
    }

    std::cout << "INFO: Iniciando apagado de ServidorRPC..." << std::endl;
    
    // 1. Pone la bandera en falso para detener el bucle 'while'
    estaCorriendo = false;

    // 2. Detiene el bloqueo de 'work()' en el servidor
    if (servidorXmlRpc) {
        servidorXmlRpc->exit();
    }

    // 3. Espera a que el hilo termine su ejecución (Join)
    if (hiloServidor && hiloServidor->joinable()) {
        hiloServidor->join();
        std::cout << "INFO: Hilo de ServidorRPC finalizado (joined)." << std::endl;
    }

    // El unique_ptr de servidorXmlRpc se encargará de destruirlo.
}

