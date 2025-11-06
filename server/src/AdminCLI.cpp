#include "AdminCLI.h"
#include "ControladorGeneral.h" 
#include <limits> 
#include <iomanip>
#include <stdexcept>


AdminCLI::AdminCLI(ControladorGeneral* controlador) 
    : refControladorGeneral(controlador) 
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
    string estado = refControladorGeneral->solicitarReporteEstadoRobot();
    cout << "\n--- ESTADO ACTUAL ---" << endl;
    cout << estado << endl;
}

void AdminCLI::mostrarMenuPrincipal() const {
    mostrarEstadoGeneral();
    
    cout << "\n--- MENU DE ADMINISTRACION ---" << endl;
    cout << "1. Conectar/Desconectar Robot (Serial)" << endl;
    cout << "2. Habilitar/Deshabilitar Acceso Remoto" << endl;
    cout << "3. Activar/Desactivar Motores" << endl;
    cout << "4. Movimiento Manual (G1, Home, Pinza)" << endl;
    cout << "5. Gestionar Usuarios y Permisos" << endl;
    cout << "6. Reportes y Logs (Actividad, Log Servidor)" << endl;
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
                refControladorGeneral->alternarConexionRobot(); 
                break;
            case '2':
                refControladorGeneral->alternarAccesoRemoto();
                break;
            case '3':
                refControladorGeneral->alternarMotores(); 
                break;
            case '4':
                menuMovimientoManual();
                break;
            case '5':
                cout << "Funcionalidad de gestion de usuarios pendiente." << endl;
                break;
            case '6':
                cout << "Reporte Administrador:\n" << refControladorGeneral->solicitarReporteAdmin() << endl;
                cout << "Reporte Log:\n" << refControladorGeneral->solicitarReporteLog() << endl;
                break;
            case 'q':
            case 'Q':
                cout << "Cerrando sesion de administrador." << endl;
                break;
            case 's':
            case 'S':
                cout << "Iniciando apagado seguro del servidor..." << endl;
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error critico en la operacion: " << e.what() << endl;
    }
}

void AdminCLI::menuMovimientoManual() {
    float x, y, z, velocidad = 10.0f;
    char sub_opcion;
    
    cout << "\n--- SUB-MENU MOVIMIENTO MANUAL ---" << endl;
    cout << "A. Mover a Home (G28)" << endl;
    cout << "B. Mover a Posicion (G1)" << endl;
    cout << "C. Activar/Desactivar Pinza (M3/M5)" << endl;
    cout << "V. Volver al Menu Principal" << endl;
    cout << "Seleccione una opcion: ";
    cin >> sub_opcion;
    
    try {
        switch (toupper(sub_opcion)) {
            case 'A':
                refControladorGeneral->irAHome(); 
                break;
            case 'B':
                cout << "Ingrese X: "; cin >> x;
                cout << "Ingrese Y: "; cin >> y;
                cout << "Ingrese Z: "; cin >> z;
                cout << "Ingrese Velocidad (opcional, default 10.0): "; 
                if (cin.peek() != '\n' && cin.peek() != EOF) cin >> velocidad;
                
                refControladorGeneral->moverRobot(x, y, z, velocidad); 
                break;
            case 'C':
                refControladorGeneral->alternarEfectorFinal(); 
                break;
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
