#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "commands.h"

void ejecutar_redireccion(char **argv, char *archivo) {
   
    int fd = open(archivo, O_CREATE | O_WRONLY);
    
    if(fd < 0){
        fprintf(2, "sh: error abriendo o creando %s\n", archivo);
        exit(1);
    }

    close(1);
    dup(fd);
    close(fd);

    exec(argv[0], argv);
    fprintf(2, "sh: exec fallo en redireccion\n");
    exit(1);
}