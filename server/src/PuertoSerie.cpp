#include "PuertoSerie.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <cerrno>

#include <unistd.h>     
#include <fcntl.h>      
#include <termios.h>    
#include <sys/select.h> 
#include <sys/time.h>   

using namespace std;

PuertoSerie::PuertoSerie(): puerto(), baudrate(115200), estado(false), fd(-1) {}

PuertoSerie::PuertoSerie(const string& dev, int baud): puerto(dev), baudrate(baud), estado(false), fd(-1) {}

PuertoSerie::~PuertoSerie() {
    try { cerrar(); } catch (...) {}
}

void PuertoSerie::setPuerto(const string& dev) {
    if (estado) throw runtime_error("No se puede cambiar puerto estando abierto el puerto");
    puerto = dev;
}

void PuertoSerie::setBaudrate(int baud) {
    if (estado) throw runtime_error("No se puede cambiar baudrate estando abierto el puerto");
    baudrate = baud;
}

speed_t PuertoSerie::mapBaud(int baud) {
    switch (baud) {
        case 1200:   return B1200;
        case 2400:   return B2400;
        case 4800:   return B4800;
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default:
            throw runtime_error("Baudrate no soportado: " + to_string(baud));
    }
}

void PuertoSerie::abrir() {
    if (puerto.empty()) {
        throw runtime_error("Puerto serie no definido (ej.: /dev/ttyACM0)");
    }
    if (estado) return;

    fd = ::open(puerto.c_str(), O_RDWR | O_NOCTTY);
    if (fd < 0) {
        throw runtime_error("No se pudo abrir el puerto: " + puerto +
                                 " (" + string(strerror(errno)) + ")");
    }

    struct termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        ::close(fd); fd = -1;
        throw runtime_error("tcgetattr() fallo: " + string(strerror(errno)));
    }

    speed_t spd = mapBaud(baudrate);
    cfsetospeed(&tty, spd);
    cfsetispeed(&tty, spd);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        ::close(fd); fd = -1;
        throw runtime_error("tcsetattr() fallo: " + string(strerror(errno)));
    }

    tcflush(fd, TCIOFLUSH);

    estado = true;
}

void PuertoSerie::escribir(const string& comando) {
    if (!estado || fd < 0) {
        throw runtime_error("Puerto no abierto para escribir");
    }
    
    ssize_t n = ::write(fd, comando.c_str(), comando.length());
    
    if (n != (ssize_t)comando.length()) {
        throw runtime_error("Fallo al escribir el comando completo en el puerto");
    }
}

void PuertoSerie::drenarHastaInactividad(int idle_timeout_ms, int overall_timeout_ms) {
    (void) overall_timeout_ms;
    if (!estado || fd < 0) return;

    auto ms_to_timeval = [](int ms) {
        timeval tv;
        tv.tv_sec  = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        return tv;
    };

    const int READ_CHUNK = 256;
    vector<char> buf(READ_CHUNK);

    while (true) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeval tv = ms_to_timeval(idle_timeout_ms);

        int rv = select(fd + 1, &set, nullptr, nullptr, &tv);
        if (rv <= 0) break; 
        ssize_t r = ::read(fd, buf.data(), READ_CHUNK);
        if (r <= 0) break; 
    }
}

string PuertoSerie::leerHastaNLineas(int n, int idle_timeout_ms, int overall_timeout_ms) {
    if (!estado || fd < 0) {
        throw runtime_error("Puerto no abierto para leer");
    }
    if (n <= 0) return {};

    string out;
    out.reserve(static_cast<size_t>(n) * 64);
    int lineas = 0;

    auto ms_to_timeval = [](int ms) {
        timeval tv;
        tv.tv_sec  = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        return tv;
    };

    const int READ_CHUNK = 256;
    vector<char> buf(READ_CHUNK);

    int restante_overall_ms = overall_timeout_ms;

    bool cr_pendiente = false;

    while (true) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeval tv = ms_to_timeval(idle_timeout_ms);

        int rv = select(fd + 1, &set, nullptr, nullptr, &tv);
        if (rv < 0) {
            throw runtime_error(string("select() fallo: ") + strerror(errno));
        } else if (rv == 0) {
            
            if (!out.empty()) {
                if (cr_pendiente) {
                    out.push_back('\n');
                    ++lineas;
                    cr_pendiente = false;
                }
                break;
            }
            restante_overall_ms -= idle_timeout_ms;
            if (restante_overall_ms <= 0) {
                throw runtime_error("Timeout esperando datos por el puerto serie");
            }
            continue;
        }

        ssize_t r = ::read(fd, buf.data(), READ_CHUNK);
        if (r < 0) {
            throw runtime_error(string("read() fallo: ") + strerror(errno));
        } else if (r == 0) {
            continue;
        }

        for (ssize_t i = 0; i < r; ++i) {
            char c = buf[static_cast<size_t>(i)];

            if (cr_pendiente) {
                if (c == '\n') {
                    out.push_back('\n');
                    ++lineas;
                    cr_pendiente = false;
                    continue;
                } else {
                    out.push_back('\n');
                    ++lineas;
                    cr_pendiente = false;
                }
            }

            if (c == '\r') {
                cr_pendiente = true;
            } else if (c == '\n') {
                out.push_back('\n');
                ++lineas;
            } else {
                out.push_back(c);
            }

            if (lineas >= n) {
                if (cr_pendiente) {
                    out.push_back('\n');
                    ++lineas;
                    cr_pendiente = false;
                }
                return out;
            }
        }
    }

    if (cr_pendiente) {
        out.push_back('\n');
        ++lineas;
        cr_pendiente = false;
    }

    return out;
}

string PuertoSerie::leerDatos() { // Para usar directamente el nombre del metodo en UML
    const int LINEAS_A_LEER = 5; 
    
    return leerHastaNLineas(LINEAS_A_LEER);
}


void PuertoSerie::cerrar() {
    if (estado && fd >= 0) {
        ::close(fd);
        fd = -1;
        estado = false;
    }
}