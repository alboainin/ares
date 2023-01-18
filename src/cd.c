#ifndef ARES_BUILTIN_CD
#define ARES_BUILTIN_CD

#include "lib.h"

extern char *builtin_str[];

int ares_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "ares: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("ares");
    }
  }
  return 1;
}

#endif