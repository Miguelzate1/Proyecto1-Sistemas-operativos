#include "kernel/types.h"
#include "user/user.h"
#include "commands.h"
#include "parser.h"

int getcmd(char *buf, int nbuf) {
  fprintf(2, "$ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0)
    return -1;
  return 0;
}

int main(void) {
  static char buf[100];

  while(getcmd(buf, sizeof(buf)) >= 0) {

    if(buf[strlen(buf)-1] == '\n')
      buf[strlen(buf)-1] = 0;

    // Comando interno exit obligatorio
    if(buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t' && buf[4]==0)
      exit(0);

    int pid = fork();

    if(pid < 0){
      fprintf(2, "sh: error en fork\n");
      continue;
    }
    
    if (pid == 0){
      Command cmd;
      
      
      if (parse_command(buf, &cmd) <= 0) {
        exit(0); 
      }

      int redir_out = -1; 
      int redir_in = -1;  
      int pipe_idx = -1;  

      
      for (int i = 0; i < cmd.argc; i++) {
        if (strcmp(cmd.argv[i], ">") == 0) redir_out = i;
        else if (strcmp(cmd.argv[i], "<") == 0) redir_in = i;
        else if (strcmp(cmd.argv[i], "|") == 0) pipe_idx = i;
      }

      
      if (pipe_idx != -1) {
        // Aquí conectaremos la lógica de tuberías (Pipes) más adelante
        fprintf(2, "sh: pipes aun no implementados\n");
        exit(1);
      } 
      else if (redir_out != -1 && redir_out + 1 < cmd.argc) {
        
        cmd.argv[redir_out] = 0; 
        char *archivo = cmd.argv[redir_out + 1];
        ejecutar_redireccion(cmd.argv, archivo);
      }
      else if (redir_in != -1 && redir_in + 1 < cmd.argc) {
        
        cmd.argv[redir_in] = 0; 
        char *archivo = cmd.argv[redir_in + 1];
        ejecutar_redireccion_entrada(cmd.argv, archivo);
      } 
      else {
        
        exec(cmd.argv[0], cmd.argv);
        fprintf(2,"sh: exec %s fallo\n", cmd.argv[0]);
        exit(1);
      }
    }
    wait(0);
  }

  exit(0);
}