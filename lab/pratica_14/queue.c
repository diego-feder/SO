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

queue_t *queue_remove (queue_t **queue, queue_t *elem) {
    queue_t *first, *queue_aux, *aux;
    // verifica se o elemento existe
    if (elem == NULL) {
        return NULL;
    }
    // verifica se a fila existe (para ela existir não deve estar vazia)
    if (*queue == NULL) {
        return NULL;
    }
    else {
        first = *queue;
        queue_aux = *queue;
        // verifica se elemento a ser removido e o primeiro da fila
        if (elem == first) {
            // verifica se a fila e' de um so elemento
            if (queue_size(*queue) == 1)
                *queue=NULL;
            else {
                queue_t *next = first->next;
                queue_t *prev = first->prev;
                next->prev = prev;
                prev->next = next;
                first = next;
                *queue = first;
            }
            elem->prev = NULL;
            elem->next = NULL;
            return elem;
        }
        // se não, procura o elemento a ser removido
        else {
            while (queue_aux->next != first) {
                aux = queue_aux->next;
                if (elem == aux) {
                    queue_t *next = aux->next;
                    queue_t *prev = aux->prev;
                    next->prev = prev;
                    prev->next = next;
                    elem->prev = NULL;
                    elem->next = NULL;
                    return elem;
                }
                queue_aux = queue_aux->next;
            }
        }
    }
    return NULL;  
}

int queue_size (queue_t *queue) {
    if (queue == NULL) // fila vazia
        return 0;
    int i = 1;
    queue_t *queue_aux = queue;
    queue_t *first = queue;
    while (queue_aux->next != first) {
        queue_aux = queue_aux->next;
        i ++;
    }
    return i;
}

void queue_print (char *name, queue_t *queue, void print_elem (void *) ) {
    queue_t *queue_aux = queue, *first = queue;
    if (queue != NULL) {
        printf("%s [", name);
        print_elem((void *)queue_aux);
        while (queue_aux->next != first) {
            queue_aux = queue_aux->next;
            print_elem((void *)queue_aux);
        }
        printf("]\n");
    }
    else
        printf("%s []\n", name);
}
