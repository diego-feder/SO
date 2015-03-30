#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16

void *threadBody(void *id)
{
   long tid = (long) id ;

   printf ("t%02ld: Olá!\n", tid) ;
   sleep (3) ;   
   printf ("t%02ld: Tchau!\n", tid) ;
   
   pthread_exit (NULL) ;
}

// Esse programa realiza as mesmas tarefas de que o programa antigo create,
// com a diferença de que agora ele usa atributos nas threads e seta as 
// threads como joinables, ou seja, de que podem ter que ser esperadas
// por qualquer outra thread que usa a função pthread_join, o que ocorre
// na task principal, no segundo for, onde ele aguarda todas as threads
// terminarem.
//
// Explicando o objetivo do parametro attr, ele tem como objetivo
// armazenar os atributos de uma thread. pthread_join foi explica
// do acima, onde a thread que chama essa função espera pelo tér-
// mino da thread que passou como parâmetro na chamada.
//
// Vale ressaltar que todas as threads posix são joinable por padrão
// e aqui nesse exemplo, o professor Carlos Mazieiro só setou o attr
// das threads para joinable, para ensinar como o processo é feito.
// No programa join.c, NULL era passado como atributo de attr na cha
// mada pthread_create. Aqui, existe um attr passado como parâmetro
// em pthread_create. Esse attr é inicializado em pthread_attr_init
// e é setado para joinable em pthread_attr_setdetachstate

int main (int argc, char *argv[])
{
   pthread_t thread [NUM_THREADS] ;
   pthread_attr_t attr ;   
   long i, status ;
   
   pthread_attr_init (&attr) ;
   pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) ;

   for(i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: criando thread %02ld\n", i) ;
      
      status = pthread_create (&thread[i], &attr, threadBody, (void *) i) ;
      if (status)
      {
         perror ("pthread_create") ;
         exit (1) ;
      }
   }
   
   for (i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: aguardando thread %02ld\n", i);
      status = pthread_join (thread[i], NULL) ;
      if (status)
      {
         perror ("pthread_join") ;
         exit (1) ;
      }
   }
   
   pthread_attr_destroy (&attr) ;
   
   pthread_exit (NULL) ;
}
