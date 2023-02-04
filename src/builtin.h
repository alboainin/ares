#ifndef ARES_BUILTIN_HEADER
#define ARES_BUILTIN_HEADER

#include "lib.h"

/*
    Function Declarations for builin shell commands:
 */
extern int ares_cd(char **args);
extern int ares_help(char **args);
extern int ares_exit(char **args);

extern char *builtin_str[] = {"cd", "help", "exit"};

extern int (*builtin_func[])(char **) = {&ares_cd, &ares_help, &ares_exit};

extern int ares_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

#endif