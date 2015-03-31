#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"

#define N 100

typedef struct filaint_t
{
   struct filaint_t *prev ;  // primeiro campo (para usar cast com queue_t)
   struct filaint_t *next ;  // segundo  campo (para usar cast com queue_t)
   int    id ;
   // outros campos podem ser acrescidos aqui...
} filaint_t ;

filaint_t item[N];
filaint_t *fila0, *fila1, *aux, *final ;

void print_elem (void *ptr)
{
   filaint_t *elem = ptr ;

   if (!elem)
      return ;

   elem->prev ? printf ("%d", elem->prev->id) : printf ("*") ;
   printf ("<%d>", elem->id) ;
   elem->next ? printf ("%d", elem->next->id) : printf ("*") ;
}

int main () {
    fila0=NULL;
    int i;
    for (i=0; i<N; i++) {
      item[i].id = i ;
      item[i].prev = NULL ;
      item[i].next = NULL ;
    }
    filaint_t *it = malloc(sizeof(struct filaint_t));
    it->id=158;
    it->prev=NULL;
    it->next=NULL;
    queue_append((queue_t **) &fila0, (queue_t *) it);
    queue_append((queue_t **) &fila0, (queue_t *) &item[1]);
    queue_append((queue_t **) &fila0, (queue_t *) &item[2]);
    printf("%d\n", queue_size((queue_t *) fila0));
    queue_print ("Saida gerada ", (queue_t*) fila0, print_elem);
    queue_remove((queue_t **) &fila0, (queue_t *) &item[1]);
    queue_remove((queue_t **) &fila0, (queue_t *) it);
    queue_remove((queue_t **) &fila0, (queue_t *) &item[2]);
    printf("%d\n", queue_size((queue_t *) fila0));
    return 0;
}
