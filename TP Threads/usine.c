#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "usine.h"
#include "entrepot.h"

#define INTERVALLE_PRODUCTION 1

void *activite_usine(void *arg){
    aofu *usine_args = (aofu *)arg;

  while(1){
    pthread_mutex_lock((usine_args->mutexStock));

    printf("Usine :\t  Stock actuel : %.2f Production d'une voiture\t\n",*(usine_args->stock));	
    if(usine_args->stock!=NULL && *(usine_args->stock)<CAPACITE_ENTREPOT){
      *(usine_args->stock) += VOITURE_PRODUITE ;
      printf("\t  Stock actuel : %.2f Production d'une voiture\t\n",*(usine_args->stock));	
      sleep(INTERVALLE_PRODUCTION);
      *(usine_args->stock) += VOITURE_PRODUITE ;
      printf("\t  Stock actuel : %.2f Production d'une voiture \t\n",*(usine_args->stock) );
      pthread_cond_signal(usine_args->cond);
      }
    pthread_cond_wait(usine_args->condUsine,usine_args->mutexStock);
    pthread_mutex_unlock(usine_args->mutexStock);

  }
}

int creation_usine(pthread_t *t,void *arg){

  return pthread_create(t,
			NULL,
			activite_usine, 
			arg);   
}
