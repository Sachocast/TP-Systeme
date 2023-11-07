#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 256
#define READ_END 0
#define WRITE_END 1

void make_message ( int num , char *message ){
    /* Rem pl i t l e b u f f e r ” message ” */
    char buftime[26] ;
    time_t timer;
    struct tm* tm_info;
    time(&timer) ;
    tm_info = localtime(&timer);
    strftime ( buftime , 26 , "%Y-%m-%d %H:%M:%S" , tm_info ) ;
    sprintf ( message , " %s %d at %s \n" , "Hello, I'm child number" , num , buftime ) ;
}

int main(int argc, char *argv[]){
  /* CREATION DU pipe -------------------------*/
  int fd[2]; /* => deux extremites : lire/´ecrire */
  int fd2[2];
  if (pipe(fd) == -1 || pipe(fd2) == -1){
    fprintf(stderr,"Pipe failed");
    return 1;
  }
  char msg[BUFFER_SIZE];

  /* fork a child process */
  pid_t pid;
  pid = fork();
  if (pid < 0) { /* error occurred */
    fprintf(stderr, "Fork Failed"); return 1;
  }

  if (pid > 0) { /* parent process : WRITE */
    int status;
   
    printf("\nChild 1 executing...\n");
    /* close the unused end of the pipe */
    close(fd[READ_END]);
    close(fd2[WRITE_END]);
    
    while(1){
        /* write to the pipe */
        make_message(1,msg);
        write(fd[WRITE_END], msg, strlen(msg)+1);
        fflush(stdout);

        read(fd2[READ_END], msg, BUFFER_SIZE);
        printf("Message received by child 1 %s",msg);
        fflush(stdout);
        sleep(1);
    }
    close(fd[WRITE_END]);
    close(fd2[READ_END]);
    wait(&status); /* Eviter le zombie ! */
    printf("\nParent finishing.\n");
  }

  else { /* child process : READ */

    printf("\n\nChild 2 executing...\n");
    /* close the unused end of the pipe */
    close(fd[WRITE_END]);
    close(fd2[READ_END]);

    while(1){
        /* read from the pipe */
        read(fd[READ_END], msg, BUFFER_SIZE);
        printf("Message received by child 2 %s",msg);
        fflush(stdout);

        make_message(2,msg);
        write(fd2[WRITE_END], msg, strlen(msg)+1);
        fflush(stdout);
    }
    close(fd[READ_END]);
    close(fd2[WRITE_END]);
  }


  return 0;
}