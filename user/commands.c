#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "commands.h"

// Maneja la redirección de salida (>)
void ejecutar_redireccion(char **argv, char *archivo) {
    // Aquí está el O_TRUNC clave que te había mencionado antes para limpiar el archivo
    int fd = open(archivo, O_CREATE | O_WRONLY | O_TRUNC);
    
    if(fd < 0){
        fprintf(2, "sh: error abriendo o creando %s\n", archivo);
        exit(1);
    }

    close(1); // Cerramos la salida estándar (pantalla)
    dup(fd);  // El archivo toma el lugar 1
    close(fd);

    exec(argv[0], argv);
    fprintf(2, "sh: exec fallo en redireccion\n");
    exit(1);
}

// Maneja la redirección de entrada (<)
void ejecutar_redireccion_entrada(char **argv, char *archivo) {
    // Aquí solo necesitamos permisos de lectura (O_RDONLY)
    int fd = open(archivo, O_RDONLY);
    
    if(fd < 0){
        fprintf(2, "sh: error abriendo %s para lectura\n", archivo);
        exit(1);
    }

    close(0); // Cerramos la entrada estándar (teclado)
    dup(fd);  // El archivo toma el lugar 0
    close(fd);

    exec(argv[0], argv);
    fprintf(2, "sh: exec fallo en redireccion de entrada\n");
    exit(1);
}