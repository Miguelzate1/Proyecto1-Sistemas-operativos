#ifndef PARSER_H
#define PARSER_H

#define MAXARGS 10

typedef struct {
    char *argv[MAXARGS];
    int argc;
} Command;

int parse_command(char *buf, Command *cmd);

#endif