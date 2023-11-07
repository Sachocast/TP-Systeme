/*--------------------------------------------------
   Version 0 d'un �change SHM System V entre deux fils.
    Les semaphores utilis�s sont POSIX !

 ----------------------------------------------------*/
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
  int i;
  char *virtualaddr;
  int shmid;
  sem_t *put;
  sem_t *get;
  key_t key;
  
  /*----- Attaching the shared mem to my address space  */
  key = ftok(argv[0],'R'); /* Generation de la key */
  shmid = shmget(key, 1024, 0644|IPC_CREAT); /* Creation du segment
                                                memoire : 1024 octets */
  if (0 > shmid){
    perror("Shared Mem creation error\n");
    exit(1);  
  }
  /* => virtualaddr will be available across fork ! */
  virtualaddr = shmat(shmid, (void*)0, 0); /* Attachement � l'espace mem du processus */

  /*--- Create POSIX Named Semaphores, and initialising with 1 */
  int init_sem_value = 1; /* Dijkstra sem */
  put = sem_open("/put", O_CREAT|O_RDWR, 0644, init_sem_value);
  get = sem_open("/get", O_CREAT|O_RDWR, 0644, init_sem_value);
        char buf[BUF_SIZE];


  switch (fork()){ /*----- child 1 */
  case -1:
    printf("Error forking child 1!\n");  exit(1);
  case 0:
    {

  
      printf("\nChild 1 executing...\n");
    
      /*Child 1 writing in shared mem */
      for(int k = 0;  k<5;k++){

        strcpy (virtualaddr, "");
        make_message(1,virtualaddr);
        strcat (virtualaddr, "");
        sleep(1);
        sem_post(get);

        /*Child 1 reading from shared mem */
        sem_wait(put);      
        strcpy (buf, virtualaddr);
        printf("Message received by child 1:%s", buf);

      }

      /*printf("Exiting child 1...\n"); */
      _exit(0);
    }
    break;
  default: break;
  }



  
  switch (fork()){ /*----- child 2 */
  case -1:
    printf("Error forking child 2!\n"); exit(1);
  case 0:
    {
        char buf[BUF_SIZE];
      /* Referring the semaphore */

      printf("\nChild 2 executing...\n");
    for(int j = 0; j<5; j++){
      /*Child 2 reading from shared memory*/
        sem_wait(get);    
        strcpy (buf, virtualaddr);
        printf("Message received by child 2:%s", buf);
 
        
        /*Child 2 writing in shared mem*/
  
        strcpy (virtualaddr, "");
        make_message(2,virtualaddr);
        strcat (virtualaddr, "");
        sleep(1);
        sem_post(put);
    }
      /*printf("Exiting child 2...\n");*/
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

  //Deleting semaphores..
  sem_unlink ("/put");
  sem_unlink ("/get");

  //Deleting Shared Memory.
if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("Erreur lors de la suppression de la mémoire partagée");
    // Gérez l'erreur si nécessaire
}
  exit(EXIT_SUCCESS);

}