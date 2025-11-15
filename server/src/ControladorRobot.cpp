#include "ControladorRobot.h"
//#include "GestorDeReportes.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace std;

ControladorRobot::ControladorRobot(const string& puerto, int baudrate, GestorDeReportes* reportes) 
    : gestorDeReportes(reportes)
{
    robot = make_unique<Robot>();
    puertoSerie = make_unique<PuertoSerie>(puerto, baudrate);
}

string ControladorRobot::conectarRobot() {
    if (puertoSerie->estaAbierto()) {
        return "INFO: El robot ya esta conectado.";
    }

    try {
        puertoSerie->abrir();
        robot->setEstadoRobot(true);

        this_thread::sleep_for(chrono::milliseconds(1500));
        
        string respuestaArranque = puertoSerie->leerHastaNLineas(5, 200, 2000);

        string respuestaG28 = enviarComando("G28\r\n");

        return respuestaArranque + "\n" + respuestaG28;

    } catch (const exception& e) {
        cerr << "ERROR al conectar el robot: " << e.what() << endl;
        robot->setEstadoRobot(false);
        return "ERROR: " + string(e.what());
    }
}

string ControladorRobot::enviarComando(const string& comando, int timeout_ms) {
    if (!robot->getEstadoRobot()) {
        throw runtime_error("Error: Robot no está conectado o activo.");
    }
    
    try {
        puertoSerie->drenarHastaInactividad(); 

        puertoSerie->escribir(comando); 

        string respuesta = puertoSerie->leerHastaNLineas(5, 200, timeout_ms); 

        if (!respuesta.empty()) {
            parsearRespuestaYActualizar(respuesta);
        }

        return respuesta;
    } catch (const exception& e) {
        cerr << "Error de comunicacion al enviar comando: " << e.what() << endl;

        return "ERROR: Fallo de comunicacion (" + string(e.what()) + ")";
    }
}

string ControladorRobot::moverAPosicion(float x, float y, float z, float velocidad) {
    stringstream ss;
    ss << "G1 X" << fixed << setprecision(2) << x 
       << " Y" << fixed << setprecision(2) << y 
       << " Z" << fixed << setprecision(2) << z 
       << " F" << fixed << setprecision(2) << velocidad << "\r\n";
    
    robot->setUltimaVelocidad(velocidad);
    
    return enviarComando(ss.str());
}

string ControladorRobot::pausar(float segundos) {
    (void)segundos;
    return "ERROR: SPEED DELAY NOT IMPLEMENTED";
}

string ControladorRobot::setModoCoordenadas(bool absoluto) {
    string comando = absoluto ? "G90\r\n" : "G91\r\n";
    string respuesta = enviarComando(comando);
    return respuesta;
}

string ControladorRobot::definirPosicionActual(float x, float y, float z, float e) {
    stringstream ss;
    ss << "G92 X" << x << " Y" << y << " Z" << z;
    if (!isnan(e)) {
        ss << " E" << e;
    }
    ss << "\r\n";
    string respuesta = enviarComando(ss.str());
    return respuesta;
}

string ControladorRobot::controlarVentilador(bool estado) {
    string comando = estado ? "M106\r\n" : "M107\r\n";
    string respuesta = enviarComando(comando);
    return respuesta;
}

string ControladorRobot::solicitarReporteEndstops() {
    return enviarComando("M119\r\n", 30000);
}

void ControladorRobot::parsearRespuestaYActualizar(const string& respuesta) {
    
    if (respuesta.find("INFO: ABSOLUTE MODE ON") != string::npos) {
        robot->setModoCoordenadas("ABSOLUTO"); 
    } else if (respuesta.find("INFO: RELATIVE MODE ON") != string::npos) {
        robot->setModoCoordenadas("RELATIVO"); 
    } 
    
    size_t pos_start = respuesta.find("CURRENT POSITION: [X:");
    if (pos_start != string::npos) {

        float x_val, y_val, z_val;
        
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

    if (respuesta.find("INFO: FAN ENABLED") != string::npos) {
        robot->setEstadoVentilador(true);
    } else if (respuesta.find("INFO: FAN DISABLED") != string::npos) {
        robot->setEstadoVentilador(false);
    }

    if (respuesta.find("INFO: MOTORS ENABLED") != string::npos) {
        robot->setEstadoMotores(true);
    } else if (respuesta.find("INFO: MOTORS DISABLED") != string::npos) {
        robot->setEstadoMotores(false);
    }
    
    if (respuesta.find("INFO: GRIPPER ON") != string::npos) {
        robot->setEstadoEfector(true);
    } else if (respuesta.find("INFO: GRIPPER OFF") != string::npos) {
        robot->setEstadoEfector(false);
    }
}

void ControladorRobot::desconectarRobot() {
    if (puertoSerie->estaAbierto()) {
        puertoSerie->cerrar();
    }
    robot->setEstadoRobot(false); 
}

string ControladorRobot::habilitarMotores(bool estado) {
    string comando = estado ? "M17\r\n" : "M18\r\n";
    return enviarComando(comando);
}

string ControladorRobot::activarEfector(bool estado) {
    string comando = estado ? "M3\r\n" : "M5\r\n";
    return enviarComando(comando, 30000);
}

string ControladorRobot::solicitarReporteEstadoRobot() {
    string respuesta = enviarComando("M114\r\n", 30000); 
    
    return respuesta;
}

string ControladorRobot::ejecutarBloqueGCode(const string& contenidoGCode) {
    if (!robot->getEstadoRobot()) {
        cerr << "ERROR: No se puede ejecutar G-Code. Robot no conectado." << endl;
        return "ERROR: Robot no conectado.";
    }
    
    stringstream ss(contenidoGCode);
    string linea;

    cout << "--- Iniciando Modo Automatico (Ejecucion de Bloque G-Code) ---" << endl;
    
    while (getline(ss, linea)) {
       
        if (linea.empty() || linea[0] == ';') continue;
        
        string comandoFinal = linea + "\r\n";
        
        try {
            string respuesta = enviarComando(comandoFinal); 

            if (respuesta.find("ERROR") != string::npos || respuesta.find("FAILURE") != string::npos) {
                return respuesta;
            }
        } catch (const exception& e) {

            return "ERROR: " + string(e.what());
        }
    }

    return "INFO: Bloque G-Code ejecutado con exito.";
}