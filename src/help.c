#ifndef ARES_BUILTIN_HELP
#define ARES_BUILTIN_HELP

#include "lib.h"

extern char *builtin_str[];

int ares_help(char **args)
{
  int i;
  printf("Ali Alboainin's ares\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < ares_num_builtins(); i++)
  {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

#endif