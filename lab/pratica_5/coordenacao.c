#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "queue.h"

#define NUM_THREADS 2 // O programa executará 2 threads
#define N 10 // o numero de elementos a serem alocados na queue
#define MAX_OPS 100 // o numero maximo de execucoes de cada thread

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
    int c_op; // operacao corrente - 0..10000
} thread_info;

// definicao do controle de acesso - alternancia estrita e peterson

int turn = 0; // variavel da vez - alternancia estrita e peterson
int wants[2] = {0, 0}; // quer - somente peterson

void enter_strict(int task) {
    while (turn != task) ;
}

void leave_strict(int task) {
    if (turn < NUM_THREADS - 1)
        turn ++;
    else
        turn = 0;
}

void enter_peterson(int task) {
    int other = 1 - task;
    wants[task] = 1;
    turn = task;
    while ((turn == task) && wants[other]) ;
}

void leave_peterson(int task) {
    wants[task] = 0;
}

// fim controle

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
void *thread_body_no_sync (void *arg) // sem mecanismos de sincronizacao
{
    thread_info *t = arg;
    int t_num = t->thread_num, o_value;
    filaint_t *q=t->queue, *e=q;
    while (t->c_op < MAX_OPS) {
        queue_remove((queue_t **) &q, (queue_t *) e);
        o_value = e->id;
        e->id = rand()%100;
        queue_append((queue_t **) &q, (queue_t *) e);
        printf("\nthread %d: tira %d, poe %d,\t", t_num, o_value, e->id);
        queue_print ("fila: ", (queue_t*) q, print_elem);
        t->c_op ++;
        printf("\n%d\n",t->c_op);
    }
    pthread_exit (NULL) ;
}
// fim sem sync

// alternancia estrita
void *thread_body_strict (void *arg) {
    thread_info *t = arg;
    int t_num = t->thread_num, o_value;
    filaint_t *q=t->queue, *e=q;
    while (t->c_op < MAX_OPS) {
        enter_strict(t_num); // espera o lock
        queue_remove((queue_t **) &q, (queue_t *) e);
        o_value = e->id;
        e->id = rand()%100;
        queue_append((queue_t **) &q, (queue_t *) e);
        printf("\nthread %d: tira %d, poe %d,\t", t_num, o_value, e->id);
        queue_print ("fila: ", (queue_t*) q, print_elem);
        printf("\n");
        t->c_op ++;
        printf("\n%d\n",t->c_op);
        leave_strict(t_num); // libera o lock
    }
    pthread_exit (NULL) ;
}
// fim alternancia estrita

void *thread_body_peterson (void *arg) // peterson
{
    thread_info *t = arg;
    int t_num = t->thread_num, o_value;
    filaint_t *q=t->queue, *e=q;
    while (t->c_op < MAX_OPS) {
        enter_peterson(t_num); // espera o lock
        queue_remove((queue_t **) &q, (queue_t *) e);
        o_value = e->id;
        e->id = rand()%100;
        queue_append((queue_t **) &q, (queue_t *) e);
        printf("\nthread %d: tira %d, poe %d,\t", t_num, o_value, e->id);
        queue_print ("fila: ", (queue_t*) q, print_elem);
        printf("\n");
        t->c_op ++;
        printf("\n%d\n",t->c_op);
        leave_peterson(t_num); // libera o lock
    }
    pthread_exit (NULL) ;
}

int main (int argc, char *argv[]) {
    srand(time(NULL)); // inicializa seed

    if (argc != 2) {
        fprintf(stderr, "Uso: ./a.out <controle> - 0: sem sincronizacao, 1: alternancia estrita, 2: peterson\n");
        return 1;
    }   

    long i, status ;
    thread_info t[NUM_THREADS];
    void *f; // ponteiro a corpo de thread a ser executado

    switch (atoi(argv[1])) {
        case 0:
            f = thread_body_no_sync;
            break;
        case 1:
            f = thread_body_strict;
            break;
        case 2:
            f = thread_body_peterson;
            break;
        default:
            fprintf(stderr, "Mecanismo invalido!\n");
            return 1;
    }

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
   printf("OK\n");

   printf("Fila inicial:\n");
   queue_print ("fila: ", (queue_t*) queue, print_elem);
   printf("3s de intervalo para leitura\n");

   // coloco as threads para executarem
   for (i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: criando thread %02ld\n", i) ;
      t[i].c_op = 0;
      t[i].thread_num=i;
      t[i].queue=queue;
      status = pthread_create (&t[i].thread_id, NULL, f, (void *) &t[i]) ;
      
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
