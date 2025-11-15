#include "ControladorGeneral.h" 
#include "AdminCLI.h"           
#include "ServidorRPC.h"        // <-- AÑADIR ESTO
#include <iostream>             
#include <thread>               // <-- AÑADIR ESTO
#include <memory>               // <-- AÑADIR ESTO (para make_unique)

using namespace std;

const string PUERTO_ROBOT = "/dev/ttyACM0"; 
const int BAUDRATE_ROBOT = 115200;
const string ARCHIVO_USUARIOS = "usuarios.db";
const int PUERTO_RPC = 8080; // Puerto para que se conecten los clientes Python

int main() {
    try {
        // 1. Crear el controlador (Lógica de Negocio)
        // Usamos make_unique para la gestión moderna de memoria
        auto controlador = make_unique<ControladorGeneral>(PUERTO_ROBOT, BAUDRATE_ROBOT, ARCHIVO_USUARIOS);

        // 2. Crear el servidor de red (Capa de Red)
        auto servidorRPC = make_unique<ServidorRPC>(controlador.get());

        // 3. Iniciar el servidor de red EN UN HILO SEPARADO
        // Si no se inicia en un hilo, bloqueará la AdminCLI
        servidorRPC->iniciar(PUERTO_RPC);

        // 4. Iniciar la CLI local (Capa de Vista Local)
        // Esto se ejecuta en el hilo principal
        AdminCLI cli(controlador.get());
        cli.iniciarSesion(); // Esto bloqueará el hilo principal hasta que el admin salga con 's' o 'q'

        // 5. Cuando la CLI termina (ej. el admin presionó 's'),
        //    iniciamos el apagado del servidor de red.
        cout << "INFO (main): AdminCLI ha terminado. Apagando servidor RPC..." << endl;
        servidorRPC->apagar();
        // El controlador se destruye aquí automáticamente (gracias a unique_ptr)
        // y su destructor llama a apagar() por si acaso.

    } catch (const exception& e) {
        cerr << "Error fatal en el servidor (main): " << e.what() << endl;
        return 1;
    }
    
    cout << "INFO (main): Servidor finalizado limpiamente." << endl;
    return 0;
}