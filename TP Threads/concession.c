/*--------------------------------
    Src2 File concession.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "concession.h"
#include <math.h>

void *activite_concession(void *arg){
    aofc *concession_args = (aofc *)arg;

    while(1){
        pthread_mutex_lock((concession_args->mutexStock));

        if(*(concession_args->stock)>0 && *(concession_args->stockConcess)<10.0){
            float tmp = 10 - *(concession_args->stockConcess);
            if(*(concession_args->stock) >= tmp){
                *(concession_args->stockConcess) += tmp;
                *(concession_args->stock) -= tmp;
            } else {
                *(concession_args->stockConcess) += *(concession_args->stock);
                *(concession_args->stock) -= *(concession_args->stock);
            }
        }
        printf("Concession %d : Stock actuel : %.2f \t", (concession_args->numero),*(concession_args->stockConcess));
        if (rand()%10 > 8 && *(concession_args->stockConcess)>0.0) {
            printf("Vente d’une voiture \t\n");
            *(concession_args->stockConcess) -= 1;
        }
        else { printf("RAS \t\n"); }
        if(concession_args->numero==0){pthread_cond_wait(concession_args->condConcess0,concession_args->mutexStock);}
        if(concession_args->numero==0){pthread_cond_signal(concession_args->condConcess1);}
        if(concession_args->numero==1){pthread_cond_wait(concession_args->condConcess1,concession_args->mutexStock);}
        if(concession_args->numero==1){pthread_cond_signal(concession_args->condConcess2);}
        if(concession_args->numero==2){pthread_cond_signal(concession_args->condUsine);}
        if(concession_args->numero==2){pthread_cond_wait(concession_args->condConcess2,concession_args->mutexStock);}
        pthread_mutex_unlock(concession_args->mutexStock);
    }
}
int creation_concession(pthread_t *t, void *arg){
    /* Creation de la concession */
    return pthread_create(t,
                            NULL,
                            activite_concession,
                            arg);
}
