#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "commands.h"


void ejecutar_redireccion(char **argv, char *archivo) {
    
    int fd = open(archivo, O_CREATE | O_WRONLY | O_TRUNC);
    
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


void ejecutar_redireccion_entrada(char **argv, char *archivo) {
    
    int fd = open(archivo, O_RDONLY);
    
    if(fd < 0){
        fprintf(2, "sh: error abriendo %s para lectura\n", archivo);
        exit(1);
    }

    close(0); 
    dup(fd);  
    close(fd);

    exec(argv[0], argv);
    fprintf(2, "sh: exec fallo en redireccion de entrada\n");
    exit(1);
}

void ejecutar_comando_complejo(char **argv, int argc) {
    int pipe_idx = -1;

    
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "|") == 0) {
            pipe_idx = i;
            break; 
        }
    }

    
    if (pipe_idx != -1) {
        argv[pipe_idx] = 0; 
        int p[2];
        
        if (pipe(p) < 0) { 
            fprintf(2, "sh: error creando pipe\n");
            exit(1);
        }

        
        if (fork() == 0) {
            close(1);       
            dup(p[1]);      
            close(p[0]);    
            close(p[1]);
            
            
            ejecutar_comando_complejo(argv, pipe_idx); 
            exit(0);
        }

        
        if (fork() == 0) {
            close(0);       
            dup(p[0]);      
            close(p[0]);    
            close(p[1]);
            
            
            ejecutar_comando_complejo(&argv[pipe_idx + 1], argc - pipe_idx - 1);
            exit(0);
        }

     
        close(p[0]);
        close(p[1]);
        wait(0); 
        wait(0); 
    } 
    
    else {
        int redir_out = -1;
        int redir_in = -1;

        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], ">") == 0) redir_out = i;
            if (strcmp(argv[i], "<") == 0) redir_in = i;
        }

        if (redir_out != -1 && redir_out + 1 < argc) {
            argv[redir_out] = 0;
            ejecutar_redireccion(argv, argv[redir_out + 1]);
        } 
        else if (redir_in != -1 && redir_in + 1 < argc) {
            argv[redir_in] = 0;
            ejecutar_redireccion_entrada(argv, argv[redir_in + 1]);
        } 
        else {
            
            exec(argv[0], argv);
            fprintf(2, "sh: exec %s fallo\n", argv[0]);
            exit(1);
        }
    }
}