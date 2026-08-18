#include "kernel/types.h"
#include "user/user.h"
#include "parser.h"

int
parse_command(char *buf, Command *cmd)
{
    char *p = buf;

    cmd->argc = 0;

    while (*p != 0) {

        // Ignorar espacios y tabulaciones
        while (*p == ' ' || *p == '\t') {
            *p = 0;
            p++;
        }

        // Llegamos al final
        if (*p == 0)
            break;

        // Evitar superar el número máximo de argumentos
        if (cmd->argc >= MAXARGS - 1)
            return -1;

        // Guardar el comienzo del argumento
        cmd->argv[cmd->argc++] = p;

        // Avanzar hasta encontrar espacio, tabulación o fin
        while (*p != 0 && *p != ' ' && *p != '\t')
            p++;
    }

    // exec() necesita que argv termine en NULL
    cmd->argv[cmd->argc] = 0;

    return cmd->argc;
}