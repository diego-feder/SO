#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "queue.h"

#define T_MAX 100 // tempo maximo de janela do processador

// definicao da fila de tarefas

typedef struct fila_tarefa_t {
    struct fila_tarefa_t *prev;
    struct fila_tarefa_t *next;
    int id;
    int inicio;
    int duracao;
    int fim;
    int prioridade_estatica;
    int prioridade_dinamica;
    int estado_atual; // 0 - nova, 1 - pronta, 2 - rodando, 3 - terminada
    int tempo_de_vida;
    int tempo_executado_total;
    int tempo_executado_quantum_atual;
    int tempo_espera;
} fila_tarefa_t;

// definicao da funcao constroi_cabecalho

char * constroi_cabecalho (fila_tarefa_t *tarefas) {
    fila_tarefa_t *topo = tarefas;
    char *cabecalho = malloc(sizeof(char)*4 + 8);
    char p[5];

    // comeco da montagem
    strcat (cabecalho, "tempo   ");
    sprintf(p, "P%d  ", topo->id);
    strcat(cabecalho, p);
    
    // adicionando cada processo
    while (tarefas->next != topo) {
        tarefas = tarefas->next;
        sprintf(p, "P%d  ", tarefas->id);
        strcat(cabecalho, p);
    }
    strcat(cabecalho,"\0");

    return cabecalho;
}

char * constroi_linha (fila_tarefa_t *prontos, fila_tarefa_t *tarefas, fila_tarefa_t *tarefa_corrente, fila_tarefa_t *executados, int t) {
    char *linha;
    // varrer cada tarefa
    return linha;
}

void fcfs (fila_tarefa_t *tarefas) {
    int t = 0;
    pthread_mutex_t processador;
    pthread_mutex_init(&processador, NULL);
    fila_tarefa_t *topo = tarefas, *tarefa_corrente, *prontos=NULL, *elemento_pronto_novo;

    printf("\n%s\n", constroi_cabecalho(tarefas));

    /*

    while (t < T_MAX) {
        //se ha uma tarefa rodando - pesquisar em todas as tarefas
        while (tarefas->next != topo) {
            if (tarefas->estado_atual == 2) { // tarefa rodando 
                // define quem e a tarefa corrente
                tarefa_corrente = tarefas;
                // fim da execucao de tarefa
                if (tarefas->tempo_de_vida == tarefa_corrente->duracao) {
                    tarefas->estado_atual = 3;
                    pthread_mutex_unlock(&processador);
                }
            }
            tarefas = tarefas->next;
        }

        tarefas = topo;

        //para cada tarefa
        while (tarefas->next != topo) {
            if (tarefas->inicio == t) {
                tarefas->estado_atual = 1;
                elemento_pronto_novo = malloc(sizeof(fila_tarefa_t)); // aloca endereco novo
                memcpy(elemento_pronto_novo, tarefas, sizeof(fila_tarefa_t));
                elemento_pronto_novo->prev = NULL;
                elemento_pronto_novo->next = NULL;
                queue_append((queue_t **) &prontos, (queue_t *) &elemento_pronto_novo);
            }
            tarefas = tarefas->next;
        }

        tarefas = topo;

        // se o processador estiver livre
        if (!pthread_mutex_trylock(&processador)) {
            if (queue_size((queue_t *) prontos) > 0) {
                tarefa_corrente = (fila_tarefa_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
                tarefa_corrente->estado_atual = 2; // tarefa rodando
                tarefa_corrente->tempo_executado_total = 0;
            }
        }
        
        t ++;
        tarefa_corrente->tempo_executado_total ++;

        printf("%s\n", constroi_linha(prontos, tarefas, tarefa_corrente, t));
    }*/
}

int main () {
    // definicao da fila de tarefas
    fila_tarefa_t *tarefas = NULL, *tarefa;
    // le dados da entrada padrao
    // exemplo: ./a.out <id> <inicio> <duracao> <prioridade>
    int c, i = 0;
    while (c != EOF) {
        tarefa = malloc(sizeof(fila_tarefa_t));
        tarefa->prev = NULL;
        tarefa->next = NULL;
        tarefa->id = i+1;
        tarefa->tempo_de_vida = 0;
        tarefa->estado_atual = 0; // tarefa nova
        scanf("%d %d %d", &tarefa->inicio, &tarefa->duracao, &tarefa->prioridade_estatica);
        fflush(stdin);
        // insercao de tarefa na fila de tarefas
        queue_append((queue_t **) &tarefas, (queue_t *) tarefa);
        c = getc(stdin);
        i ++;
    }

    fcfs(tarefas);
    
    return 0;
}
