#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "entrepot.h"
#include "usine.h"
#include <math.h>


void *activite_entrepot(void *arg){
    aofe *entrepot_args = (aofe *)arg;

    while(1){
        pthread_mutex_lock(entrepot_args->mutexStock);
        if (entrepot_args->stock != NULL) {
            printf("Entrepot :\tStock actuel : %.2f (max : 100)\t\n", *(entrepot_args->stock));
        } else {
            printf("Entrepot :\tStock acutel : NULL\t\n");
        }   
        sleep(INTERVALLE_INVENTAIRE);	
        pthread_cond_wait(entrepot_args->cond,entrepot_args->mutexStock);
        pthread_cond_signal(entrepot_args->condConcess0);
        pthread_mutex_unlock(entrepot_args->mutexStock);
        

     
    }
}

int creation_entrepot(pthread_t *t, void *arg){

    aofe *entrepot_args = (aofe *)arg;


    return pthread_create(t,
                            NULL,
                            activite_entrepot,
                            arg);
}
