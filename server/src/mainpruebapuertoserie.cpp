#include "PuertoSerie.h"
#include <iostream>

using namespace std;

void prueba_puerto_serie() {
    const string PUERTO = "/dev/ttyACM0"; // ¡REEMPLAZAR con el puerto real de tu Arduino!
    const int BAUDIOS = 115200;

    try {
        PuertoSerie ps(PUERTO, BAUDIOS);
        ps.abrir();
        cout << "OK: Puerto " << PUERTO << " abierto a " << BAUDIOS << " baudios." << endl;

        // 1. Drenar cualquier basura del buffer de inicio
        ps.drenarHastaInactividad(500, 1000); 

        // 2. Enviar M114 (Solicitar Reporte de Estado)
        string comando = "M114\r\n";
        ps.escribir(comando);
        cout << "Enviado: M114" << endl;

        // 3. Leer la respuesta (M114 devuelve varias lineas)
        string respuesta = ps.leerDatos(); 
        cout << "Respuesta recibida:\n---INICIO---\n" << respuesta << "---FIN---\n" << endl;

        // 4. Enviar un comando de accion simple
        ps.escribir("M17\r\n"); // Activar motores
        cout << "Enviado: M17" << endl;
        respuesta = ps.leerDatos();
        cout << "Respuesta M17:\n" << respuesta << endl; // Espera INFO: MOTORS ENABLED

        ps.cerrar();
    } catch (const exception& e) {
        cerr << "FALLO CRITICO en PuertoSerie: " << e.what() << endl;
    }
}

// int main() { prueba_puerto_serie(); return 0; }