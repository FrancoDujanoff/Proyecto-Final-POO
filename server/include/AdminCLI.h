#ifndef ADMINCLI_H
#define ADMINCLI_H

#include <string>
#include <iostream>
 
class ControladorGeneral; 
using namespace std;

class AdminCLI {
private:
    ControladorGeneral* refControladorGeneral;  

    void mostrarEstadoGeneral() const;
    void solicitarCredenciales();
    void mostrarMenuPrincipal() const;
    void procesarOpcion(char opcion);
    
    void menuMovimientoManual();
    void menuGestionUsuarios();

public:
    AdminCLI(ControladorGeneral* controlador);

    void iniciarSesion();
};

#endif