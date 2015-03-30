#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16 // O programa executará 16 threads

// threadBody é o corpo de execução da thread e cada thread criada 
// executará esse bloco de código, imprimindo uma mensagem de Olá
// com seu respectivo id, mantendo a execução pausada por 3 segs e
// imprimindo uma mensagem de tchau com seu respectivo id

void *threadBody (void *id)
{
   long tid = (long) id ;

   printf ("t%02ld: Olá!\n", tid) ;
   sleep (3) ;   
   printf ("t%02ld: Tchau!\n", tid) ;
   
   pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
   pthread_t thread [NUM_THREADS] ; // pthread_t é o tipo padrão para threads
   // POSIX, essa linha define um vetor de threads de tamanho NUM_THREADS: 16
   
   long i, status ; // i é um contador a ser utilizado no for e status recebe
   // o valor de retorno da função pthread_create
   
   for (i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: criando thread %02ld\n", i) ;
      
      // a função pthread_create é a função padrão para a criação de threads
      // POSIX, que recebe como argumentos um ponteiro para a thread a ser 
      // criada, os atributos desta thread, a rotina de execução da thread
      // e os parâmetros que esta rotina recebe.
      //
      // No caso tratado, &thread[i] é a thread a ser criada, NULL são os 
      // atributos desta thread, threadBody é a sua rotina de execução e i
      // é o parâmetro que esta rotina recebe
      //
      // Quando a thread é criada com sucesso, pthread_create retorna 0, 
      // caso contrário, retorna 1 e se ela não é criada, um erro é propa-
      // gado em perror("thread_create")
      
      status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ;
      
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

// Resposta da pergunta 2 do exercício:
// A ordem de criação, ativação e encerramento das threads é a mesma?
// Por que?
//
// Não e não é a mesma por que quem controla quem é criado, ativado e ence-
// rrado é o SO e isso não fica a cargo do programador.
