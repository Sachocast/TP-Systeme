/*--------------------------------
    Header File concession.h */
#ifndef CONCESSIONH
#define CONCESSIONH
#include <pthread.h>
#include <unistd.h>

#define CAPACITE_CONCESSION 10
#define INTERVALLE_VENTE 1

/* Argument de la creation d’une concession */
typedef struct argofconcession{ 
    int numero; 
    float *stock;  
    float *stockConcess;
    pthread_mutex_t *mutexStock;
    pthread_cond_t *condConcess0;  
    pthread_cond_t *condConcess1;  
    pthread_cond_t *condConcess2;  
    pthread_cond_t *condUsine;  
} aofc;
int creation_concession(pthread_t *t, void * arg);
#endif
