#!/usr/bin/env python3
"""
Script de ayuda para probar conectividad XML-RPC.

Prueba dos endpoints:
 - Mock Python server: http://localhost:8000/RPC2 (método: conectar)
 - Servidor C++:          http://localhost:8080/RPC2 (método: robot.login)

Usar PowerShell desde la raíz del repo:
  python cliente\tools\probar_conexion.py

El script imprime la respuesta o el error para cada prueba.
"""
import xmlrpc.client


def _call_method(proxy, method_name, *args):
    # Soporta nombres con punto: 'robot.login'
    target = proxy
    for part in method_name.split('.'):
        target = getattr(target, part)
    return target(*args)


def probar(url, method, *args):
    print(f"--- Probando {url} -> {method}({', '.join(map(str,args))}) ---")
    try:
        proxy = xmlrpc.client.ServerProxy(url, allow_none=True)
        resp = _call_method(proxy, method, *args)
        print("Respuesta:", resp)
    except Exception as e:
        print("Error:", type(e).__name__, e)


if __name__ == '__main__':
    MOCK = 'http://localhost:8000/RPC2'
    CPP = 'http://localhost:8080/RPC2'

    # 1) Probar mock server (se espera token)
    probar(MOCK, 'conectar', 'admin', '1234')

    # 2) Probar servidor C++ (si está corriendo en puerto 8080)
    #    El servidor C++ registra el método como 'robot.login'
    probar(CPP, 'robot.login', 'admin', '1234')
