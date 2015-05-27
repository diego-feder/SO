#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"

#define STACKSIZE 32768         /* tamanho de pilha das threads */

int id = 0;

task_t Main;
task_t *currentTask;

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

  currentTask = &Main;
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
  return id;
}

int task_switch (task_t *task) {
  task_t *aux = currentTask;
  currentTask = task;
  swapcontext (&aux->context, &task->context);
  return 0;
}

void task_exit(int exit_code) {
  task_switch(&Main);
}

int task_id () {
  return currentTask->tid;
}
