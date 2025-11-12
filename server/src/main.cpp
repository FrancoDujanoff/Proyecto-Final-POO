#include "ControladorGeneral.h" 
#include "AdminCLI.h"           
#include <iostream>             

using namespace std;

const string PUERTO_ROBOT = "/dev/ttyACM0"; 
const int BAUDRATE_ROBOT = 115200;
const string ARCHIVO_USUARIOS = "usuarios.db";

int main() {
    try {
        ControladorGeneral controlador(PUERTO_ROBOT, BAUDRATE_ROBOT, ARCHIVO_USUARIOS);

        AdminCLI cli(&controlador);

        cli.iniciarSesion();

    } catch (const exception& e) {
        cerr << "Error fatal en el servidor: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}