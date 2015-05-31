#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "queue.h"

#define STACKSIZE 32768         /* tamanho de pilha das threads */

int id = 0;

task_t Main, dispatcher;
task_t *current_task, *prontos, *tarefas;

// definicao da funcao insere_ordenado
void queue_append_sorted (task_t **queue, task_t *elem) {
    task_t *aux, *first;
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
    else {  // a fila já existe, elemento deve ser inserido na posicao de acordo com sua dynamic_prio
        aux = first = *queue;
        // o elemento novo e o de maior prioridade:
        if (elem->dynamic_prio <= aux->dynamic_prio) {
            elem->prev = aux->prev;
            aux->prev->next = elem;
            elem->next = aux;
            aux->prev = elem;
            *queue=elem;
            return;
        }
        else {
            while (aux->next != first && aux->next->dynamic_prio < elem->dynamic_prio) {
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

void dispatcher_body();

void pingpong_init() {
  // desativa o buffer da saida padrao
  setvbuf (stdout, 0, _IONBF, 0);
  // cria tarefa main
  task_t main;
  main.prev = NULL;
  main.next = NULL;
  main.tid = id;
  
  // contexto inicial
  ucontext_t context;
  getcontext(&context);
  main.context = context;
  Main = main;
  current_task = &Main;

  // cria tarefa dispatcher
  dispatcher.prev = NULL;
  dispatcher.next = NULL;
  dispatcher.tid = ++id;

  // contexto do dispatcher
  ucontext_t disp_context;
  getcontext(&disp_context);
  char *stack = malloc(STACKSIZE);
  if (stack) {
    disp_context.uc_stack.ss_sp = stack;
    disp_context.uc_stack.ss_size = STACKSIZE;
    disp_context.uc_stack.ss_flags = 0;
    disp_context.uc_link = 0;
  }

  // inicia o contexto
  makecontext(&disp_context, (void *) dispatcher_body, 0);
  dispatcher.context = disp_context;

  // inicializa fila de prontos e de tarefas
  prontos = NULL;
  tarefas = NULL;
}

int task_create (task_t *task, void (*start_routine)(void *), void *arg) {
  char *stack;
  task->prev = NULL;
  task->next = NULL;
  task->tid = ++id;
  task->state = 0;
  task->static_prio = 0;

  // criacao do contexto
  ucontext_t context;
  getcontext(&context);

  stack = malloc(STACKSIZE);
  if (stack) {
    context.uc_stack.ss_sp = stack ;
    context.uc_stack.ss_size = STACKSIZE;
    context.uc_stack.ss_flags = 0;
    context.uc_link = 0;
  }
  else {
    return -1;
  }
  makecontext (&context, (void *)(*start_routine), 1, arg); // errado, pois a funcao deveria ser generica, usando makecontext nesse caso é muita gambiarra
  task->context = context;
  // adiciona tarefa na fila de tarefas
  queue_append((queue_t **) &tarefas, (queue_t *) task);
  return id;
}

int task_switch (task_t *task) {
  task_t *aux = current_task;
  aux->state = 3;
  current_task = task;
  current_task->state = 2;
  swapcontext (&aux->context, &task->context);
  return 0;
}

void task_exit(int exit_code) {
  if (current_task != &dispatcher)
    task_switch(&dispatcher);
  else
    task_switch(&Main);
}

int task_id () {
  return current_task->tid;
}

void task_yield() {
  // devolve tarefa corrente a fila de tarefas
  if (current_task != &Main)
    queue_append((queue_t **) &tarefas, (queue_t *) current_task);
  // devolve processador ao dispatcher
  task_switch(&dispatcher);
}


// desnecessario por agora

/* 
void task_suspend (task_t *task, task_t **queue) {
  task_t *tarefa_suspensa;
  if (*queue != NULL) {
    if (task == NULL) { // suspende tarefa corrente
      tarefa_suspensa = (task_t *) queue_remove((queue_t **) &current_task, (queue_t *) current_task);
      tarefa_suspensa->state = 3; // suspensa
      queue_append((queue_t **) queue, (queue_t *) tarefa_suspensa);
    }
    else {
      tarefa_suspensa = (task_t *) queue_remove((queue_t **) &task, (queue_t *) task);
      tarefa_suspensa->state = 3; // suspensa
      queue_append((queue_t **) queue, (queue_t *) tarefa_suspensa);
    }
  }
}

void task_resume (task_t *task) {
    if (task != NULL) {
      // modifica o estado da tarefa
      task->state = 2;
      // se a tarefa estiver em uma fila, retira para adiciona-la em prontos
      if (task->next != NULL && task->prev != NULL) {
        task = (task_t *) queue_remove ((queue_t **) &task, (queue_t *) task);
        queue_append((queue_t **) &prontos, (queue_t *) task);
      }
      else {
        queue_append((queue_t **) &prontos, (queue_t *) task);
      }
  }
  else
    perror("Impossivel resumir tarefa nula\n");
}

*/

void task_setprio (task_t *task, int prio) {
  if (task != NULL)
    task->dynamic_prio = task->static_prio = prio;
  else
    current_task->dynamic_prio = current_task->static_prio = prio;
}

int task_getprio (task_t *task) {
  if (task != NULL)
    return task->static_prio;
  return current_task->static_prio;
}

task_t * scheduler() { // escalonador por prioridades
  // monta fila de prontos inicialmente
  task_t *chosen_task, *e;
  while (tarefas != NULL) {
    e = (task_t *) queue_remove((queue_t **) &tarefas, (queue_t *) tarefas);
    e->state = 1;
    queue_append_sorted(&prontos, e);
  }
  chosen_task = (task_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
  chosen_task->dynamic_prio = chosen_task->static_prio;
  // percorre tarefas restantes da lista de prontos setando prioridades dinamicas
  if (queue_size((queue_t *) prontos) > 0) {
    e = prontos;
    while (e->next != prontos) {
      e->dynamic_prio --;
      e = e->next;
    }
    e->dynamic_prio --; // diminui prioridade dinamica do ultimo elemento da fila
  }
  return chosen_task;
}

void dispatcher_body () {
  while (queue_size((queue_t *)tarefas ) > 0 || queue_size((queue_t *) prontos ) > 0) {
    task_t *next = scheduler();
    if (next) {
      task_switch(next);
    }
  }
  task_exit(0);
}
