#ifndef ADMINCLI_H
#define ADMINCLI_H

#include <string>
#include <iostream>
 
class ControladorGeneral; //Forma para declarar una clase sin incluir su header, y para no generar un bucle.

using namespace std;

class AdminCLI {
private:
    ControladorGeneral* refControladorGeneral; 

    void mostrarEstadoGeneral() const;
    void solicitarCredenciales();
    void mostrarMenuPrincipal() const;
    void procesarOpcion(char opcion);
    
    void menuMovimientoManual();

public:
    AdminCLI(ControladorGeneral* controlador);

    void iniciarSesion(); //Llama a todos los metodos privados
};

#endif