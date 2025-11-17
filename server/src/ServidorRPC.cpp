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

/**
 * @class MetodoControlado
 * @brief CLASE BASE ABSTRACTA (ADAPTADOR)
 * * Esta es una clase base personalizada que hereda de XmlRpcServerMethod.
 * Su propósito es encapsular la lógica REPETITIVA de:
 * 1. Verificar si el acceso remoto está habilitado.
 * 2. Extraer el usuario y la clave (que por convención serán params[0] y params[1]).
 * 3. Autenticar al usuario (login).
 * 4. Autorizar si el método es solo para administradores.
 * * Las clases que hereden de esta solo necesitarán implementar 
 * la lógica de negocio real en 'executeControlado'.
 */
class MetodoControlado : public XmlRpcServerMethod {
protected:
    ControladorGeneral* controlador;
    bool soloAdmin;

public:
    MetodoControlado(const std::string& name, XmlRpcServer* server, ControladorGeneral* c, bool adminOnly)
        : XmlRpcServerMethod(name, server), controlador(c), soloAdmin(adminOnly) {}

    // Implementación final del método 'execute' de la biblioteca
    void execute(XmlRpcValue& params, XmlRpcValue& result) override {
        try {
            // 1. Verificación de Acceso Remoto Global
            if (!controlador->estaAccesoRemotoHabilitado()) {
                throw XmlRpcException("Acceso remoto deshabilitado por el administrador.", 100);
            }

            // 2. Extracción de Parámetros (Convención)
            if (params.size() < 2) {
                throw XmlRpcException("Petición inválida. Se requieren al menos usuario y clave.", 101);
            }
            std::string usuario = params[0];
            std::string clave = params[1];

            // 3. Autenticación
            if (!controlador->login(usuario, clave)) {
                throw XmlRpcException("Autenticación fallida. Usuario o clave incorrectos.", 102);
            }

            // 4. Autorización (Permisos)
            if (soloAdmin && !controlador->esAdministrador(usuario)) {
                throw XmlRpcException("Permiso denegado. Se requieren privilegios de administrador.", 103);
            }

            // 5. Si todo está en orden, llamamos a la lógica específica
            //    (pasamos 'usuario' por si el log de actividad lo necesita)
            executeControlado(usuario, params, result);

        } catch (const XmlRpcException& e) {
            // Si ya es una XmlRpcException, solo la relanzamos
            throw;
        } catch (const std::exception& e) {
            // Si es una excepción estándar (ej. robot no conectado), la envolvemos
            throw XmlRpcException("Error interno del servidor: " + std::string(e.what()), 500);
        } catch (...) {
            // Captura de cualquier otro error desconocido
            throw XmlRpcException("Error fatal desconocido en el servidor.", 501);
        }
    }

    /**
     * @brief Método virtual puro que las clases hijas deben implementar.
     * Se llama DESPUÉS de que la autenticación y autorización han sido exitosas.
     */
    virtual void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) = 0;
};


// --- Adaptador para Login (Caso Especial) ---
// No puede heredar de MetodoControlado porque él ES la autenticación.
class MetodoLogin : public XmlRpcServerMethod {
    ControladorGeneral* controlador;
public:
    MetodoLogin(XmlRpcServer* s, ControladorGeneral* c) 
        : XmlRpcServerMethod("robot.login", s), controlador(c) {}

    void execute(XmlRpcValue& params, XmlRpcValue& result) override {
        if (!controlador->estaAccesoRemotoHabilitado()) {
            throw XmlRpcException("Acceso remoto deshabilitado.", 100);
        }
        if (params.size() != 2) throw XmlRpcException("robot.login requiere 2 parámetros (usuario, clave).");
        
        bool loginExitoso = controlador->login(params[0], params[1]);
        result = loginExitoso; // XmlRpcValue maneja la conversión a booleano
    }
};

// --- Adaptador para Mover Robot ---
// (Hereda de MetodoControlado, no es solo admin)
class MetodoMoverRobot : public MetodoControlado {
public:
    MetodoMoverRobot(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.mover", s, c, false) {} // false = no solo admin

    // params[0] = user, params[1] = pass
    // params[2] = x (double), params[3] = y (double), params[4] = z (double)
    // params[5] = vel (double, opcional)
    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() < 5) {
            throw XmlRpcException("robot.mover requiere 5 parámetros (user, pass, x, y, z, [vel]).");
        }
        
        float x = (double)params[2]; // XmlRpcValue maneja la conversión
        float y = (double)params[3];
        float z = (double)params[4];
        float vel = 10.0f; // Velocidad por defecto
        if (params.size() >= 6) {
            vel = (double)params[5];
        }

        std::string respuesta = controlador->moverRobot(x, y, z, vel);
        result = respuesta; // XmlRpcValue maneja la conversión a string
    }
};

// --- Adaptador para Controlar Motores ---
// (Hereda de MetodoControlado, ES solo admin)
class MetodoControlarMotores : public MetodoControlado {
public:
    MetodoControlarMotores(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarMotores", s, c, true) {} // true = solo admin

    // params[0] = user, params[1] = pass
    // params[2] = estado (bool)
    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) {
            throw XmlRpcException("robot.controlarMotores requiere 3 parámetros (user, pass, estado).");
        }
        bool estado = params[2];
        result = controlador->controlarMotores(estado);
    }
};

// --- Adaptador para Controlar Conexión (Admin) ---
class MetodoControlarConexion : public MetodoControlado {
public:
    MetodoControlarConexion(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarConexion", s, c, true) {} // true = solo admin

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("robot.controlarConexion requiere 3 parámetros (user, pass, estado).");
        bool estado = params[2];
        result = controlador->controlarConexion(estado);
    }
};

// --- Adaptador para Home (Admin) ---
class MetodoHome : public MetodoControlado {
public:
    MetodoHome(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.irAHome", s, c, true) {} // true = solo admin

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 2) throw XmlRpcException("robot.irAHome requiere 2 parámetros (user, pass).");
        result = controlador->irAHome();
    }
};

// --- Adaptador para Efector (Usuario) ---
class MetodoControlarEfector : public MetodoControlado {
public:
    MetodoControlarEfector(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.controlarEfector", s, c, false) {} // false = no solo admin

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("robot.controlarEfector requiere 3 parámetros (user, pass, estado).");
        bool estado = params[2];
        result = controlador->controlarEfector(estado);
    }
};

// --- Adaptador para Reporte de Estado (Usuario) ---
class MetodoReporteEstado : public MetodoControlado {
public:
    MetodoReporteEstado(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.solicitarReporteEstado", s, c, false) {} // false = no solo admin

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 2) throw XmlRpcException("robot.solicitarReporteEstado requiere 2 parámetros (user, pass).");
        result = controlador->solicitarReporteEstadoRobot();
    }
};

class MetodoSubirArchivo : public MetodoControlado {
public:
    MetodoSubirArchivo(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.subirArchivo", s, c, false) {} // false = cualquier usuario

    // params[2] = nombre_archivo, params[3] = contenido_texto
    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 4) throw XmlRpcException("robot.subirArchivo requiere: user, pass, nombre, contenido.");
        
        std::string nombre = params[2];
        std::string contenido = params[3];
        
        // Registramos que FUE este usuario quien subió el archivo
        std::string logMsg = "Usuario '" + usuario + "' subio archivo: " + nombre;
        // (Opcional: podríamos pasar el usuario al controlador para loguear mejor)
        
        result = controlador->guardarArchivoGCode(nombre, contenido);
    }
};

// --- Adaptador: Listar Archivos ---
class MetodoListarArchivos : public MetodoControlado {
public:
    MetodoListarArchivos(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.listarArchivos", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        // No requiere params extra
        result = controlador->listarArchivosGCode();
    }
};

// --- Adaptador: Ejecutar Archivo (Modo Automático) ---
class MetodoEjecutarArchivo : public MetodoControlado {
public:
    MetodoEjecutarArchivo(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.ejecutarArchivo", s, c, false) {} // Puede ser restringido a admin si se desea

    // params[2] = nombre_archivo
    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("robot.ejecutarArchivo requiere: user, pass, nombre.");
        
        std::string nombre = params[2];
        result = controlador->ejecutarArchivoGCode(nombre);
    }
};

// --- Adaptador: Iniciar Aprendizaje ---
class MetodoIniciarAprendizaje : public MetodoControlado {
public:
    MetodoIniciarAprendizaje(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.iniciarAprendizaje", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        // Params: user, pass
        result = controlador->iniciarAprendizaje();
    }
};

// --- Adaptador: Registrar Punto ---
class MetodoRegistrarPunto : public MetodoControlado {
public:
    MetodoRegistrarPunto(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.registrarPunto", s, c, false) {}

    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        // Params: user, pass
        result = controlador->registrarPuntoActual();
    }
};

// --- Adaptador: Guardar Trayectoria ---
class MetodoGuardarTrayectoria : public MetodoControlado {
public:
    MetodoGuardarTrayectoria(XmlRpcServer* s, ControladorGeneral* c) 
        : MetodoControlado("robot.guardarTrayectoria", s, c, false) {}

    // params[2] = nombre_archivo
    void executeControlado(const std::string& usuario, XmlRpcValue& params, XmlRpcValue& result) override {
        if (params.size() != 3) throw XmlRpcException("Requiere: user, pass, nombre_archivo.");
        std::string nombre = params[2];
        result = controlador->finalizarAprendizaje(nombre);
    }
};

// (AQUÍ SE AÑADIRÍAN MÁS ADAPTADORES PARA CADA FUNCIÓN: crearUsuario, getLista, etc.)

} // Fin del espacio de nombres anónimo


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

