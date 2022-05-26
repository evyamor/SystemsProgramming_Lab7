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
int total_commands=0;
int flag =0;
void print_history()
{
  if(flag==0){
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
    if(commands_counter>9){
    flag=1;
    commands_counter=0;
  }
  char *temp = (malloc(strlen(command)));
  strcpy(temp, command);
  history[commands_counter] = temp;
  commands_counter = commands_counter + 1;
  total_commands=total_commands+1;

}

void execute(cmdLine *pCmdLine)
{
  FILE *inp;
  FILE *out;
  int fd[2];
  pipe(fd);
  int cpid_status;
  pid_t cpid;
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
    if (pCmdLine->next != NULL)
    {
      close(1);
      dup(fd[1]);
      close(fd[1]);
    }
    execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    perror("Exec : ");
    exit(EXIT_FAILURE);
  }
  else
  {
    close(fd[1]);
    if (pCmdLine->blocking == 1)
    {
      wait(&cpid_status);
    }
    if (pCmdLine->next != NULL)
    {
      pCmdLine = pCmdLine->next;
      int cpid2_status;
      pid_t cpid2;
      cpid2 = fork();
      if (cpid2 == -1)
        perror("fork 2");
      if (cpid2 == 0)
      {
        close(0);
        dup(fd[0]);
        close(fd[0]);
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("Exec : ");
        exit(EXIT_FAILURE);
      }
      else
      {
        close(fd[0]);
        if (pCmdLine->blocking == 1)
        {
          wait(&cpid2_status);
        }
      }
    }
  }
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
  if(strcmp(cmd->arguments[0],"quit")==0){
    freeCmdLines(cmd);
    break;
    }
    else{
       if(strcmp(cmd->arguments[0],"cd")==0){
          chdir(cmd->arguments[1]);
          perror("cd : ");
         }
    else{
         execute(cmd);
        }
      update_history(line);
      freeCmdLines(cmd);
    }
  }
  for (int j = 0; j < 10; j++)
  {
    if (!history[j])
      free(history[j]);
  }
  return 0;
}