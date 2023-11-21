#ifndef USINEH
#define USINEH

#include <pthread.h>
#include <unistd.h>

#define INTERVALLE_PRODUCTION 1
#define VOITURE_PRODUITE 0.5

// Argument structure for creating the usine thread
typedef struct usine_args{
    float *stock; // Pointer to the shared stock
    pthread_mutex_t *mutexStock;
    pthread_cond_t *cond; 
    pthread_cond_t *condUsine; 
} aofu;

// Function prototypes
void *activite_usine(void *arg);
int creation_usine(pthread_t *t, void * arg);

#endif