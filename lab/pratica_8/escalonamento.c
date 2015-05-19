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
    int prioridade_estatica;
    int prioridade_dinamica;
    int estado_atual; // 0 - nova, 1 - pronta, 2 - rodando, 3 - terminada
    int tempo_de_vida;
    int tempo_executado_total;
    int tempo_executado_quantum_atual;
    int tempo_espera;
} fila_tarefa_t;

// definicao da funcao insere_ordenado

void queue_append_sorted (fila_tarefa_t **queue, fila_tarefa_t *elem) {
    fila_tarefa_t *aux, *first;
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
    else {  // a fila já existe, elemento deve ser inserido na posicao de acordo com sua duracao
        aux = first = *queue;
        // o elemento novo e de menor duracao possivel:
        if (elem->duracao < aux->duracao) {
            elem->prev = aux->prev;
            aux->prev->next = elem;
            elem->next = aux;
            aux->prev = elem;
            *queue=elem;
            return;
        }
        else {
            while (aux->next != first && aux->next->duracao < elem->duracao) {
                aux = aux->next;
            }
            elem->next = aux->next;
            aux->next->prev = elem;
            elem->prev = aux;
            aux->next = elem;
        }
        return;
    }

}

// definicao da funcao constroi_cabecalho

char * constroi_cabecalho (fila_tarefa_t *tarefas) {
    fila_tarefa_t *topo = tarefas;
    char *cabecalho = malloc(sizeof(char)*4 + 9);
    char p[5];

    // comeco da montagem
    strcat (cabecalho, "tempo    ");
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
                           queue_size((queue_t *) executados);
    if (tarefa_corrente != NULL)
        numero_elementos ++;

    char *linha = malloc(sizeof(char)*(numero_elementos*4 + 10));
    char **str = calloc(numero_elementos, sizeof(char)*(numero_elementos*4 + 1));
    char inicio_linha[10];

    // comeco da linha
    sprintf(inicio_linha, " %2d-%2d   ", t, t+1);
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
    if (tarefa_corrente != NULL) {
        topo = prontos;
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
    int t = 0, numero_processos_executados = 0, total_processos = queue_size((queue_t *) tarefas), i;
    pthread_mutex_t processador;
    pthread_mutex_init(&processador, NULL);
    fila_tarefa_t *tarefa_corrente = NULL, *prontos = NULL, *executados = NULL, *elemento_pronto_novo = NULL, *iterador = NULL;

    printf("\n%s\n", constroi_cabecalho(tarefas));

    while (t < T_MAX && numero_processos_executados < total_processos) {

        if (tarefa_corrente != NULL) { // se ha tarefa rodando
            if (tarefa_corrente->duracao == tarefa_corrente->tempo_executado_total) {
                tarefa_corrente->estado_atual = 3; // terminado
                queue_append((queue_t **) &executados, (queue_t *) tarefa_corrente);
                pthread_mutex_unlock(&processador);
                tarefa_corrente = NULL;
                numero_processos_executados ++;
            }
        }

        // varre tarefas para adicionar na fila de prontos
        i = 0;
        iterador = tarefas;
        while (i < queue_size((queue_t *) tarefas)) {
            if (iterador->inicio == t) { // processo se inicia agora
                elemento_pronto_novo = (fila_tarefa_t *) queue_remove((queue_t **) &tarefas, (queue_t *) iterador);
                queue_append((queue_t **) &prontos, (queue_t *) elemento_pronto_novo);
                i = 0;
                iterador = tarefas;
            }
            else {
                i ++;
                iterador = iterador->next;
            }
        }

        // se houver tarefas prontas
        if (queue_size((queue_t *) prontos) > 0) {
            if (!pthread_mutex_trylock(&processador)) { // se o processador estiver livre
                tarefa_corrente = (fila_tarefa_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
                tarefa_corrente->estado_atual = 2; // tarefa rodando
                tarefa_corrente->tempo_executado_total = 0;
            }
        }
        
        printf("%s\n", constroi_linha(tarefas, prontos, tarefa_corrente, executados, t));

        t ++;

        if (tarefa_corrente != NULL) {
            tarefa_corrente->tempo_executado_total ++;
        }
    }
}

void sjf (fila_tarefa_t *tarefas) {
    int t = 0, numero_processos_executados = 0, total_processos = queue_size((queue_t *) tarefas), i;
    pthread_mutex_t processador;
    pthread_mutex_init(&processador, NULL);
    fila_tarefa_t *tarefa_corrente = NULL, *prontos = NULL, *executados = NULL, *elemento_pronto_novo = NULL, *iterador = NULL;

    printf("\n%s\n", constroi_cabecalho(tarefas));

    while (t < T_MAX && numero_processos_executados < total_processos) {

        if (tarefa_corrente != NULL) { // se ha tarefa rodando
            if (tarefa_corrente->duracao == tarefa_corrente->tempo_executado_total) {
                tarefa_corrente->estado_atual = 3; // terminado
                queue_append((queue_t **) &executados, (queue_t *) tarefa_corrente);
                pthread_mutex_unlock(&processador);
                tarefa_corrente = NULL;
                numero_processos_executados ++;
            }
        }

        // varre tarefas para adicionar na fila de prontos
        i = 0;
        iterador = tarefas;
        while (i < queue_size((queue_t *) tarefas)) {
            if (iterador->inicio == t) { // processo se inicia agora
                elemento_pronto_novo = (fila_tarefa_t *) queue_remove((queue_t **) &tarefas, (queue_t *) iterador);
                queue_append_sorted(&prontos, elemento_pronto_novo);
                i = 0;
                iterador = tarefas;
            }
            else {
                i ++;
                iterador = iterador->next;
            }
        }

        // se houver tarefas prontas
        if (queue_size((queue_t *) prontos) > 0) {
            if (!pthread_mutex_trylock(&processador)) { // se o processador estiver livre
                tarefa_corrente = (fila_tarefa_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
                tarefa_corrente->estado_atual = 2; // tarefa rodando
                tarefa_corrente->tempo_executado_total = 0;
            }
        }
        
        printf("%s\n", constroi_linha(tarefas, prontos, tarefa_corrente, executados, t));

        t ++;

        if (tarefa_corrente != NULL) {
            tarefa_corrente->tempo_executado_total ++;
        }
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

    sjf(tarefas);
    
    return 0;
}
