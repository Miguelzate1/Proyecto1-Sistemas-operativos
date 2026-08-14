#include "kernel/types.h"
#include "user/user.h"
#include "commands.h"
#include "commands.c"



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

    int pid =  fork ();

    if(pid < 0){
      fprintf(2, "sh: error en fork\n");
      continue;
    }
    
    if (pid ==0){
      
      char *argv[10];
      int argc = 0;
      char *p = buf;

      while(*p != 0) {
        while(*p == ' ' || *p == '\t') { *p = 0; p++; }
        if(*p == 0) break;
        argv[argc++] = p;
        while(*p != 0 && *p != ' ' && *p != '\t') p++;
      }
      argv[argc] = 0;

      if(argc == 0) exit(0);
      
      exec(argv[0],argv);
      fprintf(2,"sh: exec %s fallo\n",buf);
      exit(1);

    }
    wait(0);
  }

  exit(0);
}