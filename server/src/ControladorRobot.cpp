#include "ControladorRobot.h"
#include "GestorDeReportes.h"
#include <sstream>
#include <iostream>
#include <iomanip>

ControladorRobot::ControladorRobot(const string& puerto, int baudrate, GestorDeReportes* reportes) 
    : gestorDeReportes(reportes)
{
    robot = make_unique<Robot>();
    puertoSerie = make_unique<PuertoSerie>(puerto, baudrate);
}

bool ControladorRobot::conectarRobot() {
    if (puertoSerie->estaAbierto()) return true;

    try {
        puertoSerie->abrir();
        robot->setEstadoRobot(true);

        puertoSerie->drenarHastaInactividad(); 

        string respuestaG28 = enviarComando("G28\r\n");
        solicitarReporteEstadoRobot(); 

        return true;
    } catch (const exception& e) {
        cerr << "ERROR al conectar el robot: " << e.what() << endl;
        robot->setEstadoRobot(false);
        return false;
    }
}

string ControladorRobot::enviarComando(const string& comando) {
    if (!robot->getEstadoRobot()) {
        throw runtime_error("Error: Robot no está conectado o activo.");
    }
    
    try {
        puertoSerie->drenarHastaInactividad(); 

        puertoSerie->escribir(comando); 

        string respuesta = puertoSerie->leerDatos(); 

        if (!respuesta.empty()) {
            parsearRespuestaYActualizar(respuesta);
        }

        return respuesta;
    } catch (const exception& e) {
        cerr << "Error de comunicacion al enviar comando: " << e.what() << endl;

        return "ERROR: Fallo de comunicacion (" + string(e.what()) + ")";
    }
}

bool ControladorRobot::moverAPosicion(float x, float y, float z, float velocidad) {
    stringstream ss;
    ss << "G1 X" << fixed << setprecision(2) << x 
       << " Y" << fixed << setprecision(2) << y 
       << " Z" << fixed << setprecision(2) << z 
       << " F" << fixed << setprecision(2) << velocidad << "\r\n";

    string respuesta = enviarComando(ss.str());

    if (respuesta.find("INFO: LINEAR MOVE") != string::npos || respuesta.find("INFO: HOMING COMPLETE") != string::npos) {
        return true;
    }
    
    if (respuesta.find("ERROR: POINT IS OUTSIDE OF WORKSPACE") != string::npos) {
        return false;
    }

    return false;
}

void ControladorRobot::parsearRespuestaYActualizar(const string& respuesta) {
    
    if (respuesta.find("INFO: ABSOLUTE MODE ON") != string::npos) {
        robot->setModoCoordenadas("ABSOLUTO"); 
    } else if (respuesta.find("INFO: RELATIVE MODE ON") != string::npos) {
        robot->setModoCoordenadas("RELATIVO"); 
    } 
    
    size_t pos_start = respuesta.find("CURRENT POSITION: [X:");
    if (pos_start != string::npos) {
        float x_val, y_val, z_val, e_val;
        
        string sub_respuesta = respuesta.substr(pos_start);
        stringstream ss(sub_respuesta);
        
        char c;
        if (ss >> c >> c >> c >> x_val &&
            ss >> c >> c >> c >> y_val &&
            ss >> c >> c >> c >> z_val)   
        {
            robot->setPosX(x_val);
            robot->setPosY(y_val);
            robot->setPosZ(z_val);
        }
    }
    
    if (respuesta.find("INFO: MOTORS ENABLED") != string::npos) {
        robot->setEstadoMotores(true);
    } else if (respuesta.find("INFO: MOTORS DISABLED") != string::npos) {
        robot->setEstadoMotores(false);
    }
    
}

void ControladorRobot::desconectarRobot() {
    if (puertoSerie->estaAbierto()) {
        puertoSerie->cerrar();
    }
    robot->setEstadoRobot(false); 
}

bool ControladorRobot::habilitarMotores(bool estado) {
    string comando = estado ? "M17\r\n" : "M18\r\n";
    string respuesta = enviarComando(comando);

    if (respuesta.find("INFO: MOTORS") != string::npos) {
        robot->setEstadoMotores(estado);
        return true;
    }
    return false;
}

bool ControladorRobot::activarEfector(bool estado) {
    string comando = estado ? "M3\r\n" : "M5\r\n";
    string respuesta = enviarComando(comando);

    if (respuesta.find("INFO: GRIPPER") != string::npos) {
        robot->setEstadoEfectorFinal(estado);
        return true;
    }
    return false;
}

string ControladorRobot::solicitarReporteEstadoRobot() {
    string respuesta = enviarComando("M114\r\n"); 
    
    return respuesta;
}

bool ControladorRobot::ejecutarBloqueGCode(const string& contenidoGCode) {

    if (!robot->getEstadoRobot()) {
        cerr << "ERROR: No se puede ejecutar G-Code. Robot no conectado." << endl;
        return false;
    }
    
    stringstream ss(contenidoGCode);
    string linea;
    bool exito = true;

    cout << "--- Iniciando Modo Automatico (Ejecucion de Bloque G-Code) ---" << endl;
    
    while (getline(ss, linea)) {
    
        
        string comandoFinal = linea + "\r\n";
        
        try {
            string respuesta = enviarComando(comandoFinal); 

            if (respuesta.find("ERROR") != string::npos || respuesta.find("FAILURE") != string::npos) {
                exito = false;
                break; 
            }
        } catch (const exception& e) {
            exito = false;
            break;
        }
    }

    return exito;
}