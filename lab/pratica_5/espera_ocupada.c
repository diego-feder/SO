#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "queue.h"

#define NUM_THREADS 2 // O programa executará 2 threads
#define N 10 // o numero de elementos a serem alocados na queue

// definicao da fila de inteiros:
typedef struct filaint_t {
   struct filaint_t *prev ;  // primeiro campo (para usar cast com queue_t)
   struct filaint_t *next ;  // segundo  campo (para usar cast com queue_t)
   int    id ;
   // outros campos podem ser acrescidos aqui...
} filaint_t ;
filaint_t item[N];
filaint_t *queue;

// definicao da estrutura de infos da thread e deste vetor - id, argumentos

typedef struct thread_info {
    pthread_t thread_id;
    int thread_num;
    filaint_t *queue;
} thread_info;

// funcao de impressao
void print_elem (void *ptr)
{
   filaint_t *elem = ptr ;

   if (!elem)
      return ;

   elem->prev ? printf ("%d", elem->prev->id) : printf ("*") ;
   printf ("<%d>", elem->id) ;
   elem->next ? printf ("%d,", elem->next->id) : printf ("*") ;
}

// a funcao de insercao e retirada de elementos da fila
void *threadBody (void *arg)
{
    thread_info *t = arg;
    int t_num = t->thread_num, o_value;
    filaint_t *q=t->queue, *e=q;
    while (1) {
        queue_remove((queue_t **) &q, (queue_t *) e);
        o_value = e->id;
        e->id = rand()%100;
        queue_append((queue_t **) &q, (queue_t *) e);
        printf("\nthread %d: tira %d, poe %d,\t", t_num, o_value, e->id);
        queue_print ("fila: ", (queue_t*) q, print_elem);
        printf("\n");
    }
    pthread_exit (NULL) ;
}

int main (int argc, char *argv[]) {
   long i, status ;
   thread_info t[NUM_THREADS];

   // incializo os itens com valores aleatorios:
   printf("inicializando o vetor de itens com valores aleatorios\n");
   for (i=0; i<N; i++) {
      item[i].id = rand()%100;
      item[i].prev = NULL;
      item[i].next = NULL;
   }
   printf("OK\n");

   // insiro os itens na fila
   printf("inserindo itens na fila\n");
   for (i=0; i<N; i++) {
      queue_append((queue_t **) &queue, (queue_t *) &item[i]);
   }
   
   // coloco as threads para executarem
   for (i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: criando thread %02ld\n", i) ;
      t[i].thread_num=i;
      t[i].queue=queue;
      status = pthread_create (&t[i].thread_id, NULL, threadBody, (void *) &t[i]) ;
      
      if (status)
      {
         perror ("pthread_create") ;
         exit (1) ;
      }
   }
   pthread_exit (NULL) ; // É necessário esse comando fora das rotinas de 
                         // execução da thread?
			 // SIM, pois no Linux, todas as threads e processos
			 // são tratadas como tasks e o processo principal
			 // tem sua task iniciada e precisa ser encerrado
}
