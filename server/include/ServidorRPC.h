#ifndef SERVIDOR_RPC_H
#define SERVIDOR_RPC_H

#include <string>
#include <thread> // Necesitaremos std::thread para ejecutar el servidor en segundo plano

namespace XmlRpc {
    class XmlRpcServer;
}

class ControladorGeneral;

/**
 * @class ServidorRPC
 * @brief Encapsula toda la lógica del servidor XML-RPC.
 * * Esta clase actúa como una fachada para la biblioteca XmlRpc++.
 * Es responsable de iniciar, ejecutar y detener el servidor de red.
 * Delega toda la lógica de negocio real al ControladorGeneral.
 */
class ServidorRPC {
private:
    // Puntero a la lógica de negocio principal
    ControladorGeneral* refControladorGeneral;

    // Puntero al servidor de la biblioteca XmlRpc++
    // Usamos unique_ptr para gestionar su ciclo de vida
    std::unique_ptr<XmlRpc::XmlRpcServer> servidorXmlRpc;

    // Puntero al hilo donde se ejecutará el servidor
    std::unique_ptr<std::thread> hiloServidor;

    // Estado de ejecución
    bool estaCorriendo;

    /**
     * @brief Método privado que se ejecuta en un hilo separado.
     * Contiene el bucle principal (work) del servidor.
     */
    void bucleDeServicio(int puerto);

    /**
     * @brief Registra todos los métodos (los adaptadores) 
     * en la instancia de XmlRpcServer.
     */
    void registrarMetodos();

public:
    /**
     * @brief Constructor.
     * @param controlador Puntero al ControladorGeneral ya inicializado.
     */
    ServidorRPC(ControladorGeneral* controlador);

    /**
     * @brief Destructor.
     * Se asegura de que el servidor se apague correctamente.
     */
    ~ServidorRPC();

    /**
     * @brief Inicia el servidor XML-RPC en un hilo separado.
     * @param puerto El puerto de red en el que escuchará el servidor (ej. 8080).
     * @return true si el hilo se lanzó correctamente, false si falló.
     */
    bool iniciar(int puerto);

    /**
     * @brief Detiene el servidor XML-RPC y cierra el hilo.
     */
    void apagar();

    ServidorRPC(const ServidorRPC&) = delete;
    ServidorRPC& operator=(const ServidorRPC&) = delete;
};

#endif // SERVIDOR_RPC_H