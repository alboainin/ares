#include "builtin.h"

#define ares_RL_BUFSIZE 1024

#define ares_TOK_BUFSIZE 64
#define ares_TOK_DELIM " \t\r\n\a"

void ares_loop(void);
char *ares_read_line(void);
char **ares_split_line(char *line);
int ares_launch(char **args);
int ares_execute(char **args);

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  ares_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

void ares_loop(void)
{
  char *line;
  char **args;
  int status;

  do
  {
    printf("> ");
    line = ares_read_line();
    args = ares_split_line(line);
    status = ares_execute(args);

    free(line);
    free(args);
  } while (status);
}

char *ares_read_line(void)
{
  int bufsize = ares_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer)
  {
    fprintf(stderr, "ares: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    // Read a charecter;
    c = getchar();

    // If we hit EOF, replace it with a null charecter and return.
    if (c == EOF || c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else
    {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate
    if (position >= bufsize)
    {
      bufsize += ares_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer)
      {
        fprintf(stderr, "ares: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **ares_split_line(char *line)
{
  int bufsize = ares_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens)
  {
    fprintf(stderr, "ares: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, ares_TOK_DELIM);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize)
    {
      bufsize += ares_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens)
      {
        fprintf(stderr, "ares: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, ares_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int ares_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0)
  {
    // Child process
    if (execvp(args[0], args) == -1)
    {
      perror("ares");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    // Error forking
    perror("ares");
  }
  else
  {
    // Parent process
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int ares_execute(char **args)
{
  int i;

  if (args[0] == NULL)
  {
    // An empty command was entered
    return 1;
  }

  for (i = 0; i < ares_num_builtins(); i++)
  {
    if (strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return ares_launch(args);
}