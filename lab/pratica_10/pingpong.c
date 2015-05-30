#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "queue.h"

#define STACKSIZE 32768         /* tamanho de pilha das threads */

int id = 0;

task_t Main, dispatcher;
task_t *current_task, *prontos;

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

  // inicializa fila de prontos
  prontos = NULL;
}

int task_create (task_t *task, void (*start_routine)(void *), void *arg) {
  char *stack;
  task->prev = NULL;
  task->next = NULL;
  task->tid = ++id;

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
  // adiciona tarefa na fila de prontos
  queue_append((queue_t **) &prontos, (queue_t *) task);
  return id;
}

int task_switch (task_t *task) {
  task_t *aux = current_task;
  current_task = task;
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

task_t * scheduler() { // escalonador fcfs
  /* fcfs - first come first served - tendo como base
     essa politica, estando todas as tarefas criadas
     em main ja na fila de prontos, basta somente re-
     tirar o elemento do topo da fila de prontos que
     é o primeiro a chegar na fila.
  */
  return (task_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
}

void task_yield() {
  // devolve tarefa corrente a fila de prontos
  if (current_task != &Main)
    queue_append((queue_t **) &prontos, (queue_t *) current_task);
  // devolve processador ao dispatcher
  task_switch(&dispatcher);
}

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

void dispatcher_body () {
  while (queue_size((queue_t *) prontos) > 0) {
    task_t *next = scheduler();
    if (next) {
      task_switch(next);
    }
  }
  task_exit(0);
}
