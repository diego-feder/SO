#include "queue.h"

typedef struct pagina_t {
    struct pagina_t *prev;
    struct pagina_t *next;
    int id;
} pagina_t;

pagina_t *ram;
pagina_t *swap;

pagina_t *busca_pagina(int id) {
    // verifica se a pagina requisitada encontra-se no swap
    pagina_t *aux = swap;
    while (aux->id != id && aux->next != swap) {
        aux = aux->next;
    }
    if (aux->id == id)
        return aux;
    else
        return NULL;
}

void fifo(pagina_t *pagina) {

}

int main (int argc, char **argv) {
    int quadros_disponiveis, quadros_maximos;
    pagina_t *pagina;
    if (argc > 2) {
        quadros_disponiveis = quadros_maximos = argv[1];
        // usa estrategia de substituicao - no caso o fifo
        
    }
}
