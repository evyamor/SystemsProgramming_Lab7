#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "LineParser.h"
extern cmdLine *parseCmdLines(const char *strLine);
extern void freeCmdLines(cmdLine *pCmdLine);
extern int replaceCmdArg(cmdLine *pCmdLine, int num, const char *newString);

char *history[10];
int commands_counter = 0;
int total_commands = 0;
int flag = 0;
void print_history()
{
  if (flag == 0)
  {
    for (int i = 0; i < commands_counter; i++)
    {
      printf("%s", history[i]);
    }
  }
  else
  {
    for (int j = commands_counter; j < 10; j++)
    {
      printf("%s", history[j]);
    }
    for (int j = 0; j < commands_counter; j++)
    {
      printf("%s", history[j]);
    }
  }
}
void update_history(char *command)
{
  if (commands_counter > 9)
  {
    flag = 1;
    commands_counter = 0;
  }
  char *temp = (malloc(strlen(command)));
  strcpy(temp, command);
  history[commands_counter] = temp;
  commands_counter = commands_counter + 1;
  total_commands = total_commands + 1;
}
int **createPipes(int nPipes)
{
  int **pipes;
  pipes = malloc(sizeof(int **));
  for (int i = 0; i < nPipes; i++)
  {
    pipes[i] = malloc(sizeof(int[2]));
    pipe(pipes[i]);
    if (pipes[i][0] == -1)
    {
      fprintf(stderr, "pipe error at pipe number : %d", i);
      perror("pipe i err");
    }
  }
  return pipes;
}
void releasePipes(int **pipes, int nPipe)
{
  for (int i = 0; i < nPipe; i++)
  {
    if (!pipes[i])
      free(pipes[i]);
  }
  free(pipes);
}
int *leftPipe(int **pipes, cmdLine *pCmdLine)
{
  if (pCmdLine->idx > 0)
  {
    return pipes[pCmdLine->idx - 1];
  }
  return NULL;
}
int *rightPipe(int **pipes, cmdLine *pCmdLine)
{
  if (pCmdLine->next != NULL)
  {
    return pipes[pCmdLine->idx + 1];
  }
  return NULL;
}
void execute(cmdLine *pCmdLine)
{
  int pipe = 0;
  int **pipes;
  if (pCmdLine->next != NULL)
  {
    cmdLine *dup = pCmdLine;
    while (pCmdLine->next != NULL)
    {
      pipe++;
      pCmdLine = pCmdLine->next;
    }
    pCmdLine = dup;
    pipes = createPipes(pipe);
  }
  int c = 0;
  int temp;
  int temp2;
  int *l_fd = NULL;
  int *r_fd = NULL;
  FILE *inp;
  FILE *out;
  int cpid_status;
  pid_t cpid;
  while (c < pipe + 1)
  {
    c++;
    cpid = fork();
    if (cpid == -1)
    {
      perror("fork \n");
      exit(EXIT_FAILURE);
    }
    if (cpid == 0)
    {
      if (pCmdLine->inputRedirect != NULL)
      {
        close(0);
        inp = fopen(pCmdLine->inputRedirect, "r");
      }
      if (pCmdLine->outputRedirect != NULL)
      {
        close(1);
        out = fopen(pCmdLine->outputRedirect, "w+");
      }
      if (strcmp(pCmdLine->arguments[0], "history") == 0)
      {
        print_history();
        exit(EXIT_SUCCESS);
        return;
      }
      if (strcmp(pCmdLine->arguments[0], "!") == 0)
      {
        int log = atoi(pCmdLine->arguments[1]);
        if (log > total_commands)
        {
          fprintf(stderr, "! Log index out of bounds. So far , %d , commands initilaized \n", total_commands);
        }
        else
        {
          update_history(history[log]);
          cmdLine *cmd;
          cmd = parseCmdLines(history[log]);
          fprintf(stderr, "executing now function %s #number : %d \n", history[log], log);
          execute(cmd);
          freeCmdLines(cmd);
        }
        exit(EXIT_SUCCESS);
        return;
      }
      if ((l_fd = leftPipe(pipes, pCmdLine)) != NULL)
      {
        close(0);
        temp = dup(l_fd[0]);
        close(l_fd[0]);
      }
      if ((r_fd = rightPipe(pipes, pCmdLine)) != NULL)
      {
        close(1);
        temp2 = dup(r_fd[1]);
        close(r_fd[1]);
      }
      execvp(pCmdLine->arguments[0], pCmdLine->arguments);
      perror("Exec : ");
      exit(EXIT_FAILURE);
    }
    else
    {
      if (l_fd != NULL)
      {
        close(l_fd[0]);
      }
      if (r_fd != NULL)
      {
        close(r_fd[1]);
      }
      if (pCmdLine->blocking == 1)
      {
        wait(&cpid_status);
      }
    }
    pCmdLine = pCmdLine->next;
  }
  releasePipes(pipes, pipe);
}

int main(int argc, char **argv)
{
  char *buf;
  char *ptr;
  char line[2048];
  cmdLine *cmd;
  while (1)
  {
    if ((buf = (char *)malloc((MAX_ARGUMENTS))) != NULL)
    {
      ptr = getcwd(buf, MAX_ARGUMENTS);
      fprintf(stderr, "The current working directory: %s \n", ptr);
    }
    if (fgets(line, 2048, stdin) != NULL)
    {
      cmd = parseCmdLines(line);
    }
    if (strcmp(cmd->arguments[0], "quit") == 0)
    {
      freeCmdLines(cmd);
      break;
    }
    else
    {
      if (strcmp(cmd->arguments[0], "cd") == 0)
      {
        chdir(cmd->arguments[1]);
        perror("cd : ");
      }
      else
      {
        execute(cmd);
      }
    }
    update_history(line);
    freeCmdLines(cmd);
  }

  for (int j = 0; j < 10; j++)
  {
    if (!history[j])
      free(history[j]);
  }
  return 0;
}