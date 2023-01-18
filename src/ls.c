#ifndef ARES_BUILTIN_LS
#define ARES_BUILTIN_LS

#include "lib.h"

int ares_ls(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "ares: expected argument to \"ls\"\n");
    }
    else
    {
        if (execlp("ls", "ls", NULL) != 0)
        {
            perror("ares");
        }
    }
    return 1;
}

#endif