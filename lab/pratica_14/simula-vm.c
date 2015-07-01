#include<stdio.h>
#include<stdlib.h>
#define MAXIMO 1000

int *ram, swap[MAXIMO], frames, available, references;

int pf_fifo = 0;

void fifo(int page) {
    int i;
    if (available < frames) { // existem paginas disponiveis em ram
        ram[available] = page;
        available ++;
    }
    else { // faz o push das paginas restantes para a esquerda
           // acrescenta page ao final da ram e incrementa pf_fifo
        for (i = 0; i < frames - 1; i ++)
            ram[i] = ram[i+1];
        ram[i] = page; // acrescenta pagina ao final
        pf_fifo ++;
    }
}

int main(int argc, char **argv) {
    if (argc != 2 && argv[1] < 0)
        return -1;

    frames = atoi(argv[1]);
    ram = malloc(sizeof(int)*frames);

    available = frames;
    references = 0;

    while ((!feof(stdin)) && (references < MAXIMO))
        scanf("%d", swap[references++]);
    
    int i = 0;
    while (i < references) {
        fifo(swap[i]);
        i ++;
    }

    printf("%d quadros, %d refs: FIFO %d\n", frames, references, pf_fifo);
}
