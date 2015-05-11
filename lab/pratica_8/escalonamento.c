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

char * constroi_linha (fila_tarefa_t *tarefas, fila_tarefa_t *prontos, fila_tarefa_t *tarefa_corrente, fila_tarefa_t *executados, int t) {
    fila_tarefa_t *topo;
    int i;

    int numero_elementos = queue_size((queue_t *) tarefas) +
                           queue_size((queue_t *) prontos) +
                           queue_size((queue_t *) tarefa_corrente) +
                           queue_size((queue_t *) executados);

    char *linha = malloc(sizeof(char)*(numero_elementos*4 + 9));
    char **str = calloc(numero_elementos, sizeof(char)*(numero_elementos*4 + 1));
    char inicio_linha[9];

    // comeco da linha
    sprintf(inicio_linha, " %d-%2d   ", t, t+1);
    strcat(linha, inicio_linha);

    // varrer cada tarefa nova
    if (queue_size((queue_t *) tarefas) > 0) {
        topo = tarefas;
        str[topo->id-1] = malloc(sizeof(char)*5);
        sprintf(str[topo->id-1], "    ");
        while (tarefas->next != topo) {
            tarefas = tarefas->next;
            str[tarefas->id-1] = malloc(sizeof(char)*5);
            sprintf(str[tarefas->id-1], "    ");
        }
        tarefas = topo;
    }

    // varre cada tarefa em prontos
    if (queue_size((queue_t *) prontos) > 0) {
        topo = prontos;
        str[topo->id-1] = malloc(sizeof(char)*5);
        sprintf(str[topo->id-1], "--  ");
        while (prontos->next != topo) {
            prontos = prontos->next;
            str[prontos->id-1] = malloc(sizeof(char)*5);
            sprintf(str[prontos->id-1], "--  ");
        }
        prontos = topo;
    }

    // faz a tarefa corrente
    if (queue_size((queue_t *) tarefa_corrente) > 0) {
        str[tarefa_corrente->id-1] = malloc(sizeof(char)*5);
        sprintf(str[tarefa_corrente->id-1], "##  ");
    }

    // varre as tarefas ja executadas
    if (queue_size((queue_t *) executados) > 0) {
        topo = executados;
        str[topo->id-1] = malloc(sizeof(char)*5);
        sprintf(str[topo->id-1], "!!  ");
        while (executados->next != topo) {
            executados = executados->next;
            str[executados->id-1] = malloc(sizeof(char)*5);
            sprintf(str[executados->id-1], "!!  ");
        }
        executados = topo;
    }

    // faz o append das tarefas e seus estados correntes
    for (i = 0; i < numero_elementos; i ++) {
        strcat(linha, str[i]);
    }

    strcat(linha, "\0");

    return linha;
}

void fcfs (fila_tarefa_t *tarefas) {
    int t = 0;
    pthread_mutex_t processador;
    pthread_mutex_init(&processador, NULL);
    fila_tarefa_t *topo = NULL, *tarefa_corrente = NULL, *prontos = NULL, *executados = NULL, *elemento_pronto_novo = NULL;

    printf("\n%s\n", constroi_cabecalho(tarefas));

    while (t < T_MAX) {

        if (tarefa_corrente != NULL) { // se ha tarefa rodando
            if (tarefa_corrente->tempo_executado_total == t) {
                tarefa_corrente->estado_atual = 3; // terminado
                queue_append((queue_t **) &executados, (queue_t *) tarefa_corrente);
                tarefa_corrente = NULL;
            }
        }

        // para cada tarefa existente - averiguar se ela comeca em t e coloca-la em prontos
        topo = tarefas;
        while (tarefas->next != topo && queue_size((queue_t *) tarefas) > 0) {
            if (tarefas->inicio == t && tarefas == topo) { // primeiro elemento
                elemento_pronto_novo = (fila_tarefa_t *) queue_remove((queue_t **) &tarefas, (queue_t *) tarefas);
                queue_append((queue_t **) &prontos, (queue_t *) elemento_pronto_novo);
                topo = tarefas;
            }
            else if (tarefas->inicio == t) {
                elemento_pronto_novo = (fila_tarefa_t *) queue_remove((queue_t **) &tarefas, (queue_t *) tarefas);
                queue_append((queue_t **) &prontos, (queue_t *) elemento_pronto_novo);
            }
            else {
                tarefas = tarefas->next;
            }
        }
        tarefas = tarefas->next;

        if (queue_size((queue_t *) tarefas) == 1) {
            if (tarefas->inicio == t) {
                elemento_pronto_novo = (fila_tarefa_t *) queue_remove((queue_t **) &tarefas, (queue_t *) tarefas);
                queue_append((queue_t **) &prontos, (queue_t *) elemento_pronto_novo);
            }
        }

        // se o processador estiver livre
        if (!pthread_mutex_trylock(&processador)) {
            if (queue_size((queue_t *) prontos) > 0) {
                tarefa_corrente = (fila_tarefa_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
                tarefa_corrente->estado_atual = 2; // tarefa rodando
                tarefa_corrente->tempo_executado_total = 0;
            }
        }
        
        printf("%s\n", constroi_linha(tarefas, prontos, tarefa_corrente, executados, t));

        t ++;
        tarefa_corrente->tempo_executado_total ++;
    }
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
