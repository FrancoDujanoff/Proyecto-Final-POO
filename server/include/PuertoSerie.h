#ifndef PUERTOSERIE_H
#define PUERTOSERIE_H

#include <string>
#include <termios.h> 

using namespace std;

class PuertoSerie {

private:
    
    string puerto;
    int baudrate;
    bool estado;
    int fd; // Es el identificador único que se utiliza para interactuar con el hardware del puerto serie a través del sistema operativo
    
    static speed_t mapBaud(int baud);
    string leerHastaNLineas(int n, int idle_timeout_ms, int overall_timeout_ms);
    void drenarHastaInactividad(int idle_timeout_ms, int overall_timeout_ms);

public:

    PuertoSerie();
    PuertoSerie(const string& dev, int baud);
    ~PuertoSerie();

    void abrir();
    void cerrar();

    void escribir(const string& comando); 

    string leerDatos(); 

    void setPuerto(const string& dev);
    void setBaudrate(int baud);
    bool pregEstado() const { return estado; }
};

#endif