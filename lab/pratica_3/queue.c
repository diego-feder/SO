#include "queue.h"
#include <stdio.h>

void queue_append (queue_t **queue, queue_t *elem) {
    queue_t *first;
    if (elem == NULL) {
        fprintf(stderr, "Elemento não existente!\n");
        return;
    }
    printf("%d\n", elem->prev);
    if (elem->prev == NULL) {
        printf("aaa\n");
    }
    printf("%d\n", elem->next);
    if (elem->next == NULL) {
        printf("aab\n");
    }
    else if (elem->prev != NULL || elem->next != NULL) {
        fprintf(stderr, "Elemento já faz parte de outra fila!\n");
        return;
    }
    if (queue == NULL) {  // criacao de fila e insercao de elemento
        first = elem;
        first->prev = elem;
        first->next = elem;
        queue=&first;
        return;
    }
    else { // a fila esta definida
        
    }
}

typedef struct filaint_t
{
   struct filaint_t *prev ;  // primeiro campo (para usar cast com queue_t)
   struct filaint_t *next ;  // segundo  campo (para usar cast com queue_t)
   int    id ;
   // outros campos podem ser acrescidos aqui...
} filaint_t ;

filaint_t item[100];
filaint_t *fila0;

int main() {
    item[0].id=158;
    item[0].next=NULL;
    item[0].prev=NULL;
    printf("%x---%x\n", item[0].prev, item[0].next);
    fila0=NULL;
    queue_append ((queue_t **) fila0, (queue_t *) &item[0]);
}
