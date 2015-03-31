#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void queue_append (queue_t **queue, queue_t *elem) {
    queue_t *aux, *first;
    if (elem == NULL) {
        fprintf(stderr, "Elemento não existente!\n");
        return;
    }
    else if (elem->prev != NULL || elem->next != NULL) {
        fprintf(stderr, "Elemento já faz parte de outra fila!\n");
        return;
    }
    if (*queue == NULL) {  // criacao de fila e insercao de elemento
        aux = elem; // define-se o primeiro elemento
        aux->prev = elem;
        aux->next = elem;
        *queue=aux;
        return;
    }
    else {  // a fila já existe, elemento deve ser inserido no final
        first = *queue;
        aux = elem;  // novo elemento a ser inserido
        aux->prev = first->prev;  // o anterior do novo elemento e' o antigo ultimo
        first->prev->next = aux;  // o proximo do antigo ultimo e' o novo elemento
        aux->next = first;  // o proximo do novo elemento e' o primeiro
        first->prev = aux;  // o anterior do primeiro e' o ultimo elemento inserido
        return;
    }
}

int queue_size (queue_t *queue) {
    int i = 1;
    queue_t *queue_aux = queue, *first = queue;
    while (queue_aux->next != first) {
        queue_aux = queue_aux->next;
        i ++;
    }
    return i;
}

void queue_print (char *name, queue_t *queue, void print_elem (void *) ) {
    queue_t *queue_aux = queue, *first = queue;
    printf("%s [", name);
    print_elem((void *)queue_aux);
    while (queue_aux->next != first) {
        queue_aux = queue_aux->next;
        print_elem((void *)queue_aux);
    }
    printf("]\n");
}
