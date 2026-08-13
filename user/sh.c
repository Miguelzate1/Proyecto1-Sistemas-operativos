#include "kernel/types.h"
#include "user/user.h"

int strlen(const char*);
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
    int pid =  fork ();

    if(pid < 0){
      fprintf(2, "sh: error en fork\n");
      continue;
    }
    
    if (pid ==0){
      if(buf [strlen(buf)-1]== '\n')
      buf [strlen(buf)-1]=0;

      char *argv[2];
      argv[0]=buf;
      argv[1]=0;
      
      exec(argv[0],argv);

      fprintf(2,"sh: exec %s fallo\n",buf);
      exit(1);

    }
    wait(0);
  }

  exit(0);
}