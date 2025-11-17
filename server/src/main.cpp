#include <iostream>             
#include <string>               
#include <memory>               // Para std::unique_ptr, std::make_unique
#include <thread>               // Para std::thread

#include "ControladorGeneral.h" // El orquestador principal
#include "AdminCLI.h"           // La vista local
#include "ServidorRPC.h"        // La vista de red
#include "BaseDeDatos.h"         // La interfaz de persistencia
#include "GestorUsuariosMySQL.h" // La implementación de persistencia

using namespace std;

// --- Constantes del Robot ---
const string PUERTO_ROBOT = "/dev/ttyACM0"; 
const int BAUDRATE_ROBOT = 115200;

// --- Constantes de Red ---
const int PUERTO_RPC = 8080; // Puerto para que se conecten los clientes Python

// --- Constantes de Base de Datos ---
// (Estas son las que creó en los pasos de MySQL)
const string DB_HOST = "localhost"; // O "127.0.0.1"
const string DB_USER = "robot_admin";
const string DB_PASS = "unaClaveMuySegura123!"; // ¡DEBE CAMBIAR ESTO POR SU CLAVE REAL!
const string DB_SCHEMA = "robot_db";


int main() {
    cout << "--- Iniciando Servidor del Robot ---" << endl;
    
    try {
        // --- 1. CAPA DE PERSISTENCIA ---
        // Creamos la instancia concreta de la base de datos
        auto miDB = make_unique<GestorUsuariosMySQL>(DB_HOST, DB_USER, DB_PASS, DB_SCHEMA);
        cout << "INFO (main): Objeto GestorUsuariosMySQL creado." << endl;

        // --- 2. CAPA DE LÓGICA (Inyección de Dependencia) ---
        // Creamos el controlador principal.
        // Inyectamos la base de datos (miDB) en el controlador.
        // 'std::move' transfiere la propiedad de 'miDB' al 'controlador'.
        auto controlador = make_unique<ControladorGeneral>(
            PUERTO_ROBOT, 
            BAUDRATE_ROBOT, 
            std::move(miDB) 
        );
        cout << "INFO (main): ControladorGeneral creado e inicializado." << endl;

        // --- 3. CAPA DE RED (Multihilo) ---
        // Creamos el servidor RPC, pasándole una referencia al controlador
        auto servidorRPC = make_unique<ServidorRPC>(controlador.get());
        cout << "INFO (main): ServidorRPC creado." << endl;

        // Iniciamos el servidor de red EN UN HILO SEPARADO.
        servidorRPC->iniciar(PUERTO_RPC);
        // (El servidor RPC ahora está escuchando en el puerto 8080 en segundo plano)

        // --- 4. CAPA DE VISTA LOCAL (Hilo Principal) ---
        // Iniciamos la CLI local en el Hilo Principal
        // Esto 'bloqueará' el hilo 'main' aquí, esperando la entrada del admin.
        AdminCLI cli(controlador.get());
        cli.iniciarSesion(); 

        // --- 5. SECUENCIA DE APAGADO ---
        // Cuando 'cli.iniciarSesion()' retorna (porque el admin presionó 's' o 'q'),
        // el hilo 'main' continúa y comienza el apagado ordenado.
        
        cout << "INFO (main): AdminCLI ha terminado. Apagando servidor RPC..." << endl;
        servidorRPC->apagar(); // Esto espera (join) a que el hilo de red termine.

    } catch (const exception& e) {
        // Captura errores fatales de inicialización 
        // (ej. no pudo conectar a MySQL o al puerto serie)
        cerr << "ERROR FATAL EN EL SERVIDOR (main): " << e.what() << endl;
        return 1; // Termina con código de error
    }
    
    cout << "INFO (main): Servidor finalizado limpiamente." << endl;
    return 0; // Salida exitosa
}