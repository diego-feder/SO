#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "queue.h"

#define BUF_SIZE 5 // tamanho do buffer
#define PRODUTORES 3
#define CONSUMIDORES 2

// definicao da fila de inteiros:
typedef struct filaint_t {
   struct filaint_t *prev ;  // primeiro campo (para usar cast com queue_t)
   struct filaint_t *next ;  // segundo  campo (para usar cast com queue_t)
   int    id ;
   // outros campos podem ser acrescidos aqui...
} filaint_t ;

filaint_t *queue;

// semaforos

sem_t s_buffer, s_item, s_vaga;

void *produtor (void *arg) {
    int pause;
    long id = (long) arg;
    while (1) {
        pause = rand() % 4;
        sleep(pause);
        sem_wait (&s_vaga);
        sem_wait (&s_buffer);
        filaint_t *e = malloc(sizeof(filaint_t));
        e->id = rand()%1000;
        queue_append((queue_t **) &queue, (queue_t *) e);
        sem_post (&s_buffer);
        sem_post (&s_item);
        printf("p%ld produziu %3d\n", id, e->id);
    }
}

void *consumidor (void *arg) {
    int pause;
    long id = (long) arg;
    while (1) {
        sem_wait (&s_item);
        sem_wait (&s_buffer);
        filaint_t *e = queue;
        queue_remove((queue_t **) &queue, (queue_t *) e);
        sem_post (&s_buffer);
        sem_post (&s_vaga);
        printf("\t\t\t\t\t\tc%ld consumiu %3d\n", id, e->id);
        pause = rand() % 4;
        sleep(pause);
    }

}

int main () {
    long i, status;
    srand(time(NULL)); // inicializa seed
    pthread_t thread [PRODUTORES+CONSUMIDORES]; // threads

    // inicializo os semaforos:
    status = sem_init(&s_buffer, 0, BUF_SIZE);
    if (status) {
        perror ("sem_init");
        exit (1);
    }

    status = sem_init(&s_vaga, 0, PRODUTORES);
    if (status) {
        perror ("sem_init");
        exit (1);
    }

    status = sem_init(&s_item, 0, 0);
    if (status) {
        perror ("sem_init");
        exit (1);
    }

    // coloco os produtores para executar
    for (i = 0; i < PRODUTORES; i ++) {
        printf("iniciando produtor %ld\n", i);
        status = pthread_create(&thread[i], NULL, produtor, (void *) i);
        if (status) {
            perror ("pthread_create");
            exit (1);
        }
    }

    // coloco os consumidores para executar
    for (i = 0; i < CONSUMIDORES; i ++) {
        printf("iniciando consumidor %ld\n", i);
        status = pthread_create(&thread[PRODUTORES+i], NULL, consumidor, (void *) i);
        if (status) {
            perror ("pthread_create");
            exit (1);
        }
    }

    return 0;
}
