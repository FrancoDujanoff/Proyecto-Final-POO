#include "AdminCLI.h"
#include "ControladorGeneral.h" 
#include <limits> 
#include <iomanip>
#include <stdexcept>

using namespace std;

AdminCLI::AdminCLI(ControladorGeneral* controlador) : refControladorGeneral(controlador) 
{
    if (!refControladorGeneral) {
        throw runtime_error("AdminCLI requiere una referencia valida a ControladorGeneral.");
    }
}

void AdminCLI::solicitarCredenciales() {
    string usuario, clave;
    bool autenticado = false;
    
    while (!autenticado) {
        cout << "\n=================================================" << endl;
        cout << "  Panel de Control del Servidor RRR (AdminCLI)" << endl;
        cout << "=================================================" << endl;
        
        cout << "Ingrese Usuario (Admin): ";
        cin >> usuario;
        cout << "Ingrese Clave: ";
        cin >> clave;
        
        if (refControladorGeneral->login(usuario, clave)) {
            if (refControladorGeneral->esAdministrador(usuario)) {
                 cout << "Autenticacion exitosa. Bienvenido, " << usuario << "." << endl;
                 autenticado = true;
            } else {
                 cout << "ERROR: Acceso solo para Administradores." << endl;
            }
        } else {
            cout << "ERROR: Usuario o clave incorrectos. Intente de nuevo." << endl;
        }
    }
}

void AdminCLI::mostrarEstadoGeneral() const {
    const Robot* robot = refControladorGeneral->getRobot(); 

    if (robot && robot->getEstadoRobot()) {
        string estado = refControladorGeneral->solicitarReporteEstadoRobot();
        cout << "\n--- ESTADO ACTUAL (ONLINE) ---" << endl;
        cout << estado << endl;
    } else {
        cout << "\n--- ESTADO ACTUAL (OFFLINE) ---" << endl;
        cout << "Robot: Desconectado" << endl;
        if (robot) { 
             cout << "Ultima Pos: [X:" << robot->getPosX() << " Y:" << robot->getPosY() << " Z:" << robot->getPosZ() << "]" << endl;
             cout << "Motores: " << (robot->getEstadoMotores() ? "ON" : "OFF") << endl;
        }
    }
}

void AdminCLI::mostrarMenuPrincipal() const {
    mostrarEstadoGeneral();
    
    cout << "\n--- MENU DE ADMINISTRACION ---" << endl;
    cout << "1. Controlar Robot (Puerto Serie)" << endl;
    cout << "2. Habilitar/Deshabilitar Acceso Remoto" << endl;
    cout << "3. Controlar Motores (M17/M18)" << endl;
    cout << "4. Movimiento Manual (G-Code Sub-Menu)" << endl;
    cout << "5. Gestionar Usuarios y Permisos" << endl;
    cout << "6. Reportes y Logs (Actividad, Log Servidor)" << endl;
    cout << "7. Controlar Ventilador (M106/M107)" << endl;
    cout << "8. Consultar Estado Endstops (M119)" << endl;
    cout << "Q. Cerrar sesion" << endl;
    cout << "S. Apagar el Servidor (Shutdown)" << endl;
    cout << "--------------------------------------------" << endl;
}

void AdminCLI::iniciarSesion() {
    solicitarCredenciales(); 

    char opcion;
    do {
        mostrarMenuPrincipal();
        cout << "Seleccione una opcion: ";
        
        if (!(cin >> opcion)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcion = ' '; 
        }

        procesarOpcion(opcion);

    } while (opcion != 'q' && opcion != 'Q' && opcion != 's' && opcion != 'S');
    
    if (opcion == 's' || opcion == 'S') {
        refControladorGeneral->shutdownServidor(); 
    }
}

void AdminCLI::procesarOpcion(char opcion) {
    try {
        switch (opcion) {
            case '1':
            {
                cout << "Desea (a)ctivar o (d)esactivar la conexion con el robot? ";
                char sub_op;
                cin >> sub_op;
                if(tolower(sub_op) == 'a') {
                    cout << refControladorGeneral->controlarConexion(true) << endl;
                } else if (tolower(sub_op) == 'd') {
                    cout << refControladorGeneral->controlarConexion(false) << endl;
                }
                break;
            }
            case '2':
            {
                refControladorGeneral->alternarAccesoRemoto();
                break;
            }
            case '3':
            {
                cout << "Desea (a)ctivar o (d)esactivar los motores? ";
                char sub_op;
                cin >> sub_op;
                if(tolower(sub_op) == 'a') {
                    cout << refControladorGeneral->controlarMotores(true) << endl;
                } else if (tolower(sub_op) == 'd') {
                    cout << refControladorGeneral->controlarMotores(false) << endl;
                }
                break;
            }
            case '4':
            {
                menuMovimientoManual();
                break;
            }
            case '5':
            {
                menuGestionUsuarios();
                break;
            }
            case '6':
            {
                cout << "\n--- REPORTE DE ADMINISTRADOR (Estado Robot Actual) ---" << endl;
                cout << refControladorGeneral->solicitarReporteAdmin() << endl;
                
                cout << "\n--- LOG DEL SERVIDOR (" << /* Opcional: pasar el nombre del archivo */ ") ---" << endl;
                cout << refControladorGeneral->solicitarReporteLog() << endl;
                cout << "------------------------------------------" << endl;
                break;
            }
            case '7':
            {
                cout << "Desea (a)ctivar o (d)esactivar el ventilador? ";
                char sub_op;
                cin >> sub_op;
                if(tolower(sub_op) == 'a') {
                    cout << refControladorGeneral->controlarVentilador(true) << endl;
                } else if (tolower(sub_op) == 'd') {
                    cout << refControladorGeneral->controlarVentilador(false) << endl;
                }
                break;
            }
            case '8':
            {
                cout << "--- REPORTE ENDSTOPS (M119) ---" << endl;
                cout << refControladorGeneral->solicitarReporteEndstops() << endl;
                cout << "---------------------------------" << endl;
                break;
            }
            case 'q':
            case 'Q':
            {
                cout << "Cerrando sesion de administrador." << endl;
                break;
            }
            case 's':
            case 'S':
            {
                cout << "Iniciando apagado seguro del servidor..." << endl;
                break;
            }
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error critico en la operacion: " << e.what() << endl;
    }
}

void AdminCLI::menuMovimientoManual() {
    float x, y, z, segundos, velocidad = 10.0f;
    char sub_opcion;
    
    cout << "\n--- SUB-MENU MOVIMIENTO MANUAL ---" << endl;
    cout << "A. Mover a Home (G28)" << endl;
    cout << "B. Mover a Posicion (G1)" << endl;
    cout << "C. Controlar Pinza (M3/M5)" << endl;
    cout << "D. Pausar (G4)" << endl;
    cout << "E. Definir Posicion Actual (G92)" << endl;        
    cout << "F. Cambiar Modo Coordenadas (G90/G91)" << endl; 
    cout << "V. Volver al Menu Principal" << endl;
    cout << "Seleccione una opcion: ";
    cin >> sub_opcion;
    
    try {
        switch (toupper(sub_opcion)) {
            case 'a':
            case 'A':
            {
                cout << refControladorGeneral->irAHome() << endl; 
                break;
            }
            case 'b':
            case 'B':
            {
                cout << "Ingrese X: "; cin >> x;
                cout << "Ingrese Y: "; cin >> y;
                cout << "Ingrese Z: "; cin >> z;
                cout << "Ingrese Velocidad (opcional, default 10.0): "; 
    
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                
                string vel_input;
                
                getline(cin, vel_input); 

                if (!vel_input.empty()) {
                    try {
                        velocidad = stof(vel_input);
                    } catch (const exception& e) {
                        cout << "Valor de velocidad invalido, usando default 10.0" << endl;
                    }
                }
                
                cout << refControladorGeneral->moverRobot(x, y, z, velocidad) << endl;
                break;
            }
            case 'c':
            case 'C':
            {
                cout << "Desea (a)ctivar o (d)esactivar la pinza? ";
                char sub_op;
                cin >> sub_op;
                if(tolower(sub_op) == 'a') {
                    cout << refControladorGeneral->controlarEfector(true) << endl;
                } else if (tolower(sub_op) == 'd') {
                    cout << refControladorGeneral->controlarEfector(false) << endl;
                }
                break;
            }
            case 'd':
            case 'D':
            {
                cout << "Ingrese segundos para pausar: "; cin >> segundos;
                cout << refControladorGeneral->pausar(segundos) << endl;
                break;
            }
            case 'e':
            case 'E':
            {
                cout << "Ingrese nueva X actual: "; cin >> x;
                cout << "Ingrese nueva Y actual: "; cin >> y;
                cout << "Ingrese nueva Z actual: "; cin >> z;
                cout << refControladorGeneral->definirPosicionActual(x, y, z) << endl;
                break;
                break;
            }
            case 'f':
            case 'F':
            {
                cout << "Seleccione modo: (a)bsoluto o (r)elativo? ";
                char modo;
                cin >> modo;
                if (tolower(modo) == 'a') {
                    refControladorGeneral->setModoCoordenadas(true);
                } else if (tolower(modo) == 'r') {
                    refControladorGeneral->setModoCoordenadas(false);
                }
                break;
            }
            case 'v':
            case 'V':
                break;
            default:
                cout << "Opcion de movimiento no valida." << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error de Movimiento: " << e.what() << endl;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void AdminCLI::menuGestionUsuarios() {
    char sub_opcion;
    string username, password;
    
    cout << "\n--- SUB-MENU GESTION DE USUARIOS ---" << endl;
    cout << "A. Listar todos los usuarios" << endl;
    cout << "B. Crear nuevo usuario" << endl;
    cout << "C. Eliminar usuario" << endl;
    cout << "V. Volver al Menu Principal" << endl;
    cout << "Seleccione una opcion: ";
    cin >> sub_opcion;

    // Limpiamos el buffer de entrada para getline
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    try {
        switch (toupper(sub_opcion)) {
            case 'A':
            {
                // Llamamos al nuevo método en ControladorGeneral
                cout << refControladorGeneral->getListaUsuarios() << endl;
                break;
            }
            case 'B':
            {
                cout << "Ingrese nuevo username: ";
                getline(cin, username);
                cout << "Ingrese nueva password: ";
                getline(cin, password);
                
                cout << "Es administrador? (s/n): ";
                char esAdminInput;
                cin >> esAdminInput;
                bool esAdmin = (tolower(esAdminInput) == 's');

                if (refControladorGeneral->crearNuevoUsuario(username, password, esAdmin)) {
                    cout << "INFO: Usuario creado exitosamente." << endl;
                } else {
                    cout << "ERROR: No se pudo crear el usuario." << endl;
                }
                break;
            }
            case 'C':
            {
                cout << "Ingrese username a ELIMINAR: ";
                getline(cin, username);

                if (refControladorGeneral->eliminarUnUsuario(username)) {
                    cout << "INFO: Usuario eliminado exitosamente." << endl;
                } else {
                    cout << "ERROR: No se pudo eliminar el usuario." << endl;
                }
                break;
            }
            case 'V':
                break;
            default:
                cout << "Opcion no valida." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error en gestion de usuarios: " << e.what() << endl;
    }
}