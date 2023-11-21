/*--------------------------------
    Header File entrepot.h */
#ifndef ENTREPOTH
#define ENTREPOTH
#include <pthread.h>
#include <unistd.h>

#define CAPACITE_ENTREPOT 100
#define INTERVALLE_INVENTAIRE 1

/* Argument de la creation d’une entrepot */
typedef struct argofentrepot{ 
    int numero; 
    float *stock;
    pthread_mutex_t *mutexStock;
    pthread_cond_t *cond;
    pthread_cond_t *condConcess0;  
    } aofe;
int creation_entrepot(pthread_t *t, void * arg);
#endif
