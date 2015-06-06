#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include "pingpong.h"
#include "queue.h"

#define STACKSIZE 32768         /* tamanho de pilha das threads */

int id = 0;

task_t Main, dispatcher;
task_t *current_task, *prontos;

struct sigaction action;
struct itimerval timer;

void handler (int signum) {
  if (current_task->type == 1) {
    current_task->dynamic_quantum --;
    if (current_task->dynamic_quantum == 0) {
      task_yield();
    }
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
  main.type = 1;
  
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
  dispatcher.type = 0;

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

  // handler
  action.sa_handler = handler;
  sigemptyset (&action.sa_mask) ;
  action.sa_flags = 0 ;
  if (sigaction (SIGALRM, &action, 0) < 0)
  {
    perror ("Erro em sigaction: ") ;
    exit (1) ;
  }

  // timer
  timer.it_value.tv_usec = 100 ;      // primeiro disparo, em micro-segundos
  timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
  timer.it_interval.tv_usec = 1000 ;   // disparos subsequentes, em micro-segundos
  timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos

  // arma o temporizador ITIMER_REAL (vide man setitimer)
  if (setitimer (ITIMER_REAL, &timer, 0) < 0)
  {
    perror ("Erro em setitimer: ") ;
    exit (1) ;
  }

}

int task_create (task_t *task, void (*start_routine)(void *), void *arg) {
  char *stack;
  task->prev = NULL;
  task->next = NULL;
  task->tid = ++id;
  task->state = 1;
  task->type = 1;
  task->dynamic_quantum = task->quantum = 20;

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
  queue_append((queue_t **) &prontos, (queue_t *) task);
  return id;
}

int task_switch (task_t *task) {
  task_t *aux = current_task;
  aux->state = 1;
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

task_t * scheduler() { // escalonador por prioridades
  // monta fila de prontos inicialmente
  return (task_t *) queue_remove((queue_t **) &prontos, (queue_t *) prontos);
}

void dispatcher_body () {
  while (queue_size((queue_t *) prontos ) > 0) {
    task_t *next = scheduler();
    if (next) {
      next->dynamic_quantum = next->quantum;
      task_switch(next);
    }
  }
  task_exit(0);
}

void task_yield() {
  // devolve tarefa corrente a fila de tarefas
  if (current_task != &Main)
    queue_append((queue_t **) &prontos, (queue_t *) current_task);
  // devolve processador ao dispatcher
  task_switch(&dispatcher);
}
