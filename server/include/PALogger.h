#ifndef PALOGGER_H
/// Corrige los placeholders "cadena (string)" por std::string, agrega tipos y
/// firmas con const& y retorna la ruta de log.
class PALogger
{
public:
// Constructors/Destructors
PALogger();
virtual ~PALogger();


// Niveles de log básicos
void info(const std::string& modulo, const std::string& mensaje);
void warning(const std::string& modulo, const std::string& mensaje);


// Devuelve la ruta del archivo de log (si se usa archivo)
std::string getLogFilePath() const;


// Eventos generales
void logEvento(const std::string& modulo, const std::string& mensaje);


// Traza de petición/respuesta
void logPeticion(const std::string& timestamp,
const std::string& usuario,
const std::string& nodo,
const std::string& peticion,
const std::string& respuesta);


private:
// Private attributes
std::string logFIle; // se respeta el nombre del UML


public:
// Public attribute accessor methods
/// Set the value of logFIle
/// @param value the new value of logFIle
void setLogFIle(const std::string& value)
{
logFIle = value;
}


/// Get the value of logFIle
/// @return the value of logFIle
std::string getLogFIle() const
{
return logFIle;
}


private:
void initAttributes();
};


#endif // PALOGGER_H