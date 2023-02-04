#include "builtin.h"

#define ares_RL_BUFSIZE 1024

#define ares_TOK_BUFSIZE 64
#define ares_TOK_DELIM " \t\r\n\a"

void ares_loop(void);
char *ares_read_line(void);
char **ares_split_line(char *line);
int ares_launch(char **args);
int ares_execute(char **args);
int count_args(char **args);

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ANSI_COLOR_RED_BOLD "\x1b[1;6;31m"
#define ANSI_COLOR_GREEN_BOLD "\x1b[32m"
#define ANSI_COLOR_YELLOW_BOLD "\x1b[33m"
#define ANSI_COLOR_BLUE_BOLD "\x1b[34m"
#define ANSI_COLOR_MAGENTA_BOLD "\x1b[35m"
#define ANSI_COLOR_CYAN_BOLD "\x1b[36m"

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

  /* Prints hostname of the local device *EXPERIMENTAL FUNCTION* */

  char hostname[HOST_NAME_MAX + 1];
  gethostname(hostname, HOST_NAME_MAX + 1);

  do
  {
    printf(ANSI_COLOR_RED_BOLD "%s ~> ", hostname);
    printf(ANSI_COLOR_RESET);

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

#define MAX_ALIAS_NAME_LENGTH 100
#define MAX_ALIAS_VALUE_LENGTH 100

typedef struct alias
{
  char name[MAX_ALIAS_NAME_LENGTH];
  char value[MAX_ALIAS_VALUE_LENGTH];
  struct alias *next;
} alias_t;

alias_t *alias_list = NULL;

void add_alias(char *name, char *value)
{
  alias_t *new_alias = malloc(sizeof(alias_t));
  strncpy(new_alias->name, name, MAX_ALIAS_NAME_LENGTH);
  strncpy(new_alias->value, value, MAX_ALIAS_VALUE_LENGTH);
  new_alias->next = alias_list;
  alias_list = new_alias;
}

char *get_alias_value(char *name)
{
  alias_t *current = alias_list;
  while (current != NULL)
  {
    if (strcmp(current->name, name) == 0)
    {
      return current->value;
    }
    current = current->next;
  }
  return NULL;
}

char **build_alias_args(char *alias_value, char **args)
{
  char **alias_args = malloc(sizeof(char *) * (count_args(args)) + MAX_ALIAS_VALUE_LENGTH);
  int index = 0;
  char *token = strtok(alias_value, " ");
  while (token != NULL)
  {
    alias_args[index] = token;
    index++;
    token = strtok(NULL, " ");
  }
  for (int i = 1; i < count_args(args); i++)
  {
    alias_args[index] = args[i];
    index++;
  }
  alias_args[index] = NULL;
  return alias_args;
}

int ares_execute(char **args)
{
  if (args[0] == NULL)
  {
    return 1;
  }

  // Check for alias command
  if (strcmp(args[0], "alias") == 0)
  {
    if (args[1] == NULL || args[2] == NULL)
    {
      fprintf(stderr, "alias: missing argument(s)\n");
      return 1;
    }
    add_alias(args[1], args[2]);
    return 1;
  }

  // Check if the command is an alias
  char *alias_value = get_alias_value(args[0]);
  if (alias_value != NULL)
  {
    char **alias_args = build_alias_args(alias_value, args);
    return ares_launch(alias_args);
  }

  int i;

  for (i = 0; i < ares_num_builtins(); i++)
  {
    if (strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return ares_launch(args);
}

int count_args(char **args)
{
  int count = 0;
  while (args[count] != NULL)
  {
    count++;
  }
  return count;
}
