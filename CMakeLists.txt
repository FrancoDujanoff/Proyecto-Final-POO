cmake_minimum_required(VERSION 3.15)
project(ServidorRobot CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir directorios
include_directories(server/include lib)

# Archivos fuente del servidor
set(SERVER_SOURCES
    server/src/main.cpp
    server/src/AdminCLI.cpp
    server/src/BaseDeDatos.cpp
    server/src/ControladorGeneral.cpp
    server/src/ControladorRobot.cpp
    server/src/GestorDeArchivos.cpp
    server/src/GestorDeReportes.cpp
    server/src/GestorUsuarios.cpp
    server/src/MensajeServidor.cpp
    server/src/PALogger.cpp
    server/src/PuertoSerie.cpp
    server/src/Robot.cpp
    server/src/ServidorRPC.cpp
    server/src/UsuarioServidor.cpp
)

# Archivos fuente de XML-RPC
set(XMLRPC_SOURCES
    lib/XmlRpcClient.cpp
    lib/XmlRpcDispatch.cpp
    lib/XmlRpcServer.cpp
    lib/XmlRpcServerConnection.cpp
    lib/XmlRpcServerMethod.cpp
    lib/XmlRpcSocket.cpp
    lib/XmlRpcSource.cpp
    lib/XmlRpcUtil.cpp
    lib/XmlRpcValue.cpp
)

# Crear ejecutable
add_executable(servidor_robot ${SERVER_SOURCES} ${XMLRPC_SOURCES})

# Linkear bibliotecas de Windows
if(WIN32)
    target_link_libraries(servidor_robot ws2_32)
endif()

# Copiar usuarios.db a carpeta de compilacion
add_custom_command(TARGET servidor_robot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/server/usuarios.db
    $<TARGET_FILE_DIR:servidor_robot>/usuarios.db
)

# Mensajes de compilacion
message(STATUS "Proyecto: ${PROJECT_NAME}")
message(STATUS "Estandar C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Archivos servidor: ${SERVER_SOURCES}")
message(STATUS "Archivos XML-RPC: ${XMLRPC_SOURCES}")
