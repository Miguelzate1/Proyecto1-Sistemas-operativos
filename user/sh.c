#include "kernel/types.h"
#include "user/user.h"

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
    fprintf(2, "Comando recibido: %s", buf);
  }
  
  exit(0);
}