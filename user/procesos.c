#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int x = 10;
    
    int pid = fork();
    
    if(pid < 0){
        printf("Error en fork\n");
        exit(1);
    }

    if(pid == 0){
        // Código que ejecuta el HIJO
        x = x + 5;
        printf("HIJO -> PID=%d, x=%d\n", getpid(), x);
        exit(0);
    }

    // Código que ejecuta el PADRE
    wait(0); // Espera a que el hijo termine
    x = x + 20;
    printf("PADRE -> PID=%d, x=%d\n", getpid(), x);
    
    exit(0);
}
