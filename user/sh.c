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

      
      ejecutar_comando_complejo(cmd.argv, cmd.argc);
      
      
      exit(0);
    }
    
    wait(0);
  }

  exit(0);
}