#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

queue_t *first;

void queue_append (queue_t **queue, queue_t *elem) {
    queue_t *aux, *queue_aux;
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
        first = malloc(sizeof(struct queue_t));
        first->next = aux;
        *queue=aux;
        return;
    }
    else { // a fila já existe, elemento deve ser inserido no final
        queue_aux = *queue;
        aux = elem;
        while (queue_aux->next != first->next) {
            queue_aux = queue_aux->next;
        }
        aux->next = queue_aux->next;  // o elemento a ser inserido, o proximo dele aponta ao comeco da fila
        queue_aux->next->prev = aux;  // o comeco da fila aponta o anterior ao novo elemento inserido
        aux->prev = queue_aux; // o anterior de aux e o antigo ultimo elemento
        queue_aux->next = aux; // o proximo elemento do antigo ultimo elemento e aux
        *queue=aux->next;
        return;
    }
}

int queue_size (queue_t *queue) {
    int i = 1;
    queue_t *queue_aux = queue;
    while (queue_aux->next != first->next) {
        queue_aux = queue_aux->next;
        i ++;
    }
    return i;
}
