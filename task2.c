#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc,char**argv){
int flag=0;
if(argc!=1){
  if(strcmp(argv[1],"-d")==0)
    flag=1;
}
int fd[2],inpL;
pid_t child1_pid;
pid_t child2_pid;
int c1Status;
int c2Status;
char *command1[3];
char *command2[4];
char *temp = (malloc(strlen("ls")+1));
strcpy(temp, "ls\0");
command1[0]=temp;
char *temp2 = (malloc(strlen("-l")+1));
strcpy(temp2, "-l\0");
command1[1]=temp2;
char *temp3 = (malloc(strlen("tail")+1));
strcpy(temp3, "tail\0");
command2[0]=temp3;
char *temp4 = (malloc(strlen("-n")+1));
strcpy(temp4, "-n\0");
command2[1]=temp4;
char *temp5 = (malloc(strlen("2")+1));
strcpy(temp5, "2\0");
command2[2]=temp5;
command1[2]=malloc(strlen("\0"));
command1[2]='\0';
command2[3]=malloc(strlen("\0"));
command2[3]='\0';
fprintf(stderr,"%s %s \n",command1[0],command1[1]);
fprintf(stderr,"%s %s %s \n",command2[0],command2[1],command2[2]);
pipe(fd);
child1_pid=fork();
if(child1_pid==-1){
  perror("fork 1 \n");
  exit(EXIT_FAILURE);
} 
if(child1_pid==0){
   if(flag==1){
    fprintf(stderr,"Child1 > redirecting stdout to the write end of the pipe \n");
  }
  close(1);
  dup(fd[1]);
  close(fd[1]);
  if(flag==1){
    fprintf(stderr,"Child1 > going to execute cmd \n");
  }
  execvp(command1[0],command1);
  perror("exec 1 \n");
  exit(0);
}
else{
  wait(&c1Status);
  if(flag==1){
    fprintf(stderr,"Parent_Process > closing the write end of the pipe \n");
  }
  close(fd[1]);
  if(flag==1){
    fprintf(stderr,"Parent_Process > forking \n");
  }
  child2_pid=fork();
  if(child2_pid==-1){
  perror("fork 2 \n");
  exit(EXIT_FAILURE);
  }
  if(child2_pid==0){
     if(flag==1){
    fprintf(stderr,"Child2 > redirecting stdin to the read end of the pipe \n");
  }
    close(0);
    dup(fd[0]);
    close(fd[0]);
     if(flag==1){
    fprintf(stderr,"Child2 > going to execute cmd \n");
  }
    execvp(command2[0],command2);
    perror("exec 2 \n");
    exit(0);
  }
  else{
     if(flag==1){
      fprintf(stderr,"Parent_Process > closing the read end of the pipe \n");
      }
     close(fd[0]);
     if(flag==1){
     fprintf(stderr,"Parent_Process > waiting for child processes to terminate \n");
     }
     wait(&c2Status);
      if(flag==1){
      fprintf(stderr,"Parent_Process > exiting \n");
     }
    } 
  }
  free(command1[0]);
  free(command1[1]);
  free(command2[0]);
  free(command2[1]);
  free(command2[2]);
  free(command1[2]);
  free(command2[3]);
  return 0;
  }