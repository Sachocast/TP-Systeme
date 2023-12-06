#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <time.h>
#define BUF_SIZE 256

void make_message ( int num , char *message ){
    char buftime[26] ;
    time_t timer;
    struct tm* tm_info;
    time(&timer) ;
    tm_info = localtime(&timer);
    strftime ( buftime , 26 , "%Y-%m-%d %H:%M:%S" , tm_info ) ;
    sprintf ( message , " %s %d at %s \n" , "Hello, I'm child number" , num , buftime ) ;
}


int main(int argc, char *argv[]){
  int i;
  char *virtualaddr;
  int shmid;
  sem_t *put;
  sem_t *get;
  key_t key;
  
  key = ftok(argv[0],'R');
  shmid = shmget(key, 1024, 0644|IPC_CREAT); 
  if (0 > shmid){
    perror("Shared Mem creation error\n");
    exit(1);  
  }
  virtualaddr = shmat(shmid, (void*)0, 0); 

  int init_sem_value = 1; 
  put = sem_open("/put", O_CREAT|O_RDWR, 0644, init_sem_value);
  get = sem_open("/get", O_CREAT|O_RDWR, 0644, init_sem_value);
        char buf[BUF_SIZE];


  switch (fork()){ 
  case -1:
    printf("Error forking child 1!\n");  exit(1);
  case 0:
    {

  
      printf("\nChild 1 executing...\n");
    
      for(int k = 0;  k<5;k++){

        strcpy (virtualaddr, "");
        make_message(1,virtualaddr);
        strcat (virtualaddr, "");
        sleep(1);
        sem_post(get);

        sem_wait(put);      
        strcpy (buf, virtualaddr);
        printf("Message received by child 1:%s", buf);

      }

      _exit(0);
    }
    break;
  default: break;
  }



  
  switch (fork()){
  case -1:
    printf("Error forking child 2!\n"); exit(1);
  case 0:
    {
        char buf[BUF_SIZE];

      printf("\nChild 2 executing...\n");
    for(int j = 0; j<5; j++){
        sem_wait(get);    
        strcpy (buf, virtualaddr);
        printf("Message received by child 2:%s", buf);
 
        
  
        strcpy (virtualaddr, "");
        make_message(2,virtualaddr);
        strcat (virtualaddr, "");
        sleep(1);
        sem_post(put);
    }
      _exit(EXIT_SUCCESS);
    }
    break;  
  default:
    break;
  }


  for(i=0;i<=2;i++){
    if (wait(NULL) == -2){
      printf("Error waiting.\n");
      exit(EXIT_FAILURE);
    }
  }
  printf("Parent finishing.\n");

  sem_unlink ("/put");
  sem_unlink ("/get");

if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("Erreur lors de la suppression de la mémoire partagée");
}
  exit(EXIT_SUCCESS);

}