#include<stdio.h>
#include<stdlib.h>
#define MAXIMO 1000000

int *ram, swap[MAXIMO], vetor[MAXIMO], frames, occupied, references, swap_counter;

int pf_fifo = 0;

void fifo(int page, int swap_position) {
  int swapped_page = ram[0], i = 0;
  while (i < frames - 1) {
    ram[i] = ram[i+1];
    i ++;
  }
  ram[i] = page;
  swap[swap_position] = swapped_page;
  pf_fifo ++;
}

int main(int argc, char **argv) {
  int i;

  frames = atoi(argv[1]);
  ram = malloc(sizeof(int)*frames);
  occupied = 0;
  swap_counter = 0;

  // inicializa ram e swap
  i = 0;
  while (i < frames)
    ram[i++] = -1;

  references = 0;
  while ((!feof(stdin)) && (references < MAXIMO))
    scanf("%d\n", &vetor[references++]);

  i = 0;
  while (i < references) {
    int j, existe_ram=0,existe_swap=0;
    if (occupied < frames) { // ram nao esta cheia
      for (j = 0; j < frames; j ++)
        if (ram[j] == vetor[i])
          existe_ram = 1;
      if (!existe_ram) {
        ram[occupied] = vetor[i];
        occupied ++;
      }
    }
    else {
      for (j = 0; j < frames; j ++)
        if (ram[j] == vetor[i])
          existe_ram = 1;
      if (!existe_ram) {
        for (j = 0; j < swap_counter; j++) {
          if(swap[j] == vetor[i]) {
            existe_swap = 1;
            break;
          }
        }
        if (existe_swap)
          fifo(swap[j], j);
        else {
          fifo(vetor[i], swap_counter);
          swap_counter ++;
        }
      }
    }
    i ++;
  }
  printf("%d frames, %d references, FIFO: %d\n", frames, references, pf_fifo);
}
