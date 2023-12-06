#ifndef USINEH
#define USINEH

#include <pthread.h>
#include <unistd.h>

#define INTERVALLE_PRODUCTION 1
#define VOITURE_PRODUITE 0.5

typedef struct usine_args{
    float *stock; 
    pthread_mutex_t *mutexStock;
    pthread_cond_t *cond; 
    pthread_cond_t *condUsine; 
} aofu;

void *activite_usine(void *arg);
int creation_usine(pthread_t *t, void * arg);

#endif