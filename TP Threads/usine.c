/*
  Fichier : usine.c
  Version 2
 */
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
  /* Creation de l'usine en tant que thread 
   => le paramètre t est modifié (si le thread est créé) 
   => on rend le code de retour de la creation
  */
  
    
  return pthread_create(t,
			NULL,
			activite_usine, // routine associée au thread "=" son code
			arg);   // argument de la routine activite_usine
                                  // pour l'instant YENAPAS
}
