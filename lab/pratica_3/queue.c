#include "queue.h"
#include <stdio.h>

void queue_append (queue_t **queue, queue_t *elem) {
    if (elem == NULL) {
        fprintf(stderr, "Elemento não existente!\n");
        return;
    }
    else if (elem->prev != NULL || elem->next != NULL) {
        fprintf(stderr, "Elemento já faz parte de outra fila!\n");
        return;
    }
    if (queue == NULL) {  // criacao de fila e insercao de elemento
        elem->prev = elem;
        elem->next = elem;
        return;
    }
    else { // a fila esta definida
        
    }
}
