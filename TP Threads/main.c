#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "concession.h"
#include "entrepot.h"
#include "usine.h"

int creation_usine(pthread_t *t, void *arg);
int creation_entrepot(pthread_t *t, void *arg);


#define NB_CONCESSION 3
int main(int argc, char*argv[]){
  float stock = 0.0;
  float stockConcess[NB_CONCESSION];
  pthread_mutex_t mutexStock;
  pthread_cond_t cond;
  pthread_cond_t condConcess0;
  pthread_cond_t condConcess1;
  pthread_cond_t condConcess2;
  pthread_cond_t condUsine;
  pthread_mutex_init(&mutexStock, NULL);
  pthread_cond_init(&cond, NULL);
  pthread_cond_init(&condConcess0, NULL);
  pthread_cond_init(&condConcess1, NULL);
  pthread_cond_init(&condConcess2, NULL);
  pthread_cond_init(&condUsine, NULL);
  for (int i = 0; i < NB_CONCESSION; i++) {
    stockConcess[i] = 0;
  }   
  int r = 1; 
  int i;
  pthread_t entrepot;
  pthread_t concession[NB_CONCESSION];
  pthread_t usine;

  aofu *u = malloc(sizeof(aofu));
  u->stock = &stock;
  u->mutexStock = &mutexStock;
  u->cond = &cond;
  u->condUsine = &condUsine;
  r = creation_usine(&usine,(void *) u);
  if (r != 0)
    fprintf(stderr,"Usine echec !");
  aofe *e = malloc(sizeof(aofe));
  e->stock = &stock;
  e->mutexStock = &mutexStock;
  e->cond = &cond;
  e->condConcess0 = &condConcess0;
  r = creation_entrepot(&entrepot, (void *) e );
  if (r != 0)
    fprintf(stderr,"Entrepot echec !");

  for (i=0; i<NB_CONCESSION ; i++){
    aofc *p = malloc(sizeof(aofc));
    p->numero = i;
    p->stock = &stock;
    p->stockConcess = &stockConcess[i];
    p->mutexStock = &mutexStock;
    p->condConcess0 = &condConcess0;
    p->condConcess1 = &condConcess1;
    p->condConcess2 = &condConcess2;
    p->condUsine = &condUsine;
    r = creation_concession(&(concession[i]), (void *) p);
    if (r != 0){
      fprintf(stderr,"Concession echec !");
      free(p);
    }
  }
  pthread_join(usine,NULL);
  pthread_join(entrepot,NULL);
  for (i=0; i<NB_CONCESSION ; i++){
    pthread_join(concession[i],NULL);
  }
  pthread_mutex_destroy(&mutexStock);
  return 0;
}

