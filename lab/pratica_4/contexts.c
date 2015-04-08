#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

#define STACKSIZE 32768		/* tamanho de pilha das threads */
#define _XOPEN_SOURCE 600	/* para compilar no MacOS */

ucontext_t ContextPing, ContextPong, ContextMain;

/*****************************************************/

void BodyPing (void * arg)
{
   int i ;

   printf ("%s iniciada\n", (char *) arg) ;
   
   for (i=0; i<4; i++)
   {
      printf ("%s %d\n", (char *) arg, i) ;
      swapcontext (&ContextPing, &ContextPong); // vai executar o contexto pong, salvando o corrente em ping
   }	
   printf ("%s FIM\n", (char *) arg) ;

   swapcontext (&ContextPing, &ContextMain) ;
}

/*****************************************************/

void BodyPong (void * arg)
{
   int i ;

   printf ("%s iniciada\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s %d\n", (char *) arg, i) ;
      swapcontext (&ContextPong, &ContextPing); // vai executar o contexto ping, salvando o corrente em pong
   }
   printf ("%s FIM\n", (char *) arg) ;

   swapcontext (&ContextPong, &ContextMain) ;
}

/*****************************************************/

int main (int argc, char *argv[])
{
   char *stack ;

   printf ("Main INICIO\n");

   getcontext (&ContextPing); // Inicializa ContextPong para o contexto corrente

   stack = malloc (STACKSIZE) ;
   if (stack)
   {
      ContextPing.uc_stack.ss_sp = stack ; // a stack do contexto armazenado em ping
      ContextPing.uc_stack.ss_size = STACKSIZE; // o tamanho dessa stack
      ContextPing.uc_stack.ss_flags = 0; // flags da mesma
      ContextPing.uc_link = 0; // link para o proximo contexto a ser executado - NULO
   }
   else
   {
      perror ("Erro na criação da pilha: ");
      exit (1);
   }

   makecontext (&ContextPing, (void*)(*BodyPing), 1, "\tPing"); // associa a funcao BodyPing ao contexto armazenado em ContextPing

   getcontext (&ContextPong); // Inicializa ContextPong para o contexto corrente

   stack = malloc (STACKSIZE) ;
   if (stack) // inicializa os campos do contexto
   {
      ContextPong.uc_stack.ss_sp = stack ;
      ContextPong.uc_stack.ss_size = STACKSIZE;
      ContextPong.uc_stack.ss_flags = 0;
      ContextPong.uc_link = 0;
   }
   else
   {
      perror ("Erro na criação da pilha: ");
      exit (1);
   }

   makecontext (&ContextPong, (void*)(*BodyPong), 1, "\t\tPong"); // associa a funcao bodypong ao ContextPong

   swapcontext (&ContextMain, &ContextPing); // salva o contexto de execução corrente em ContextMain, troca pelo ContextPing associado à função bodyping
   swapcontext (&ContextMain, &ContextPong); // salva o contexto de execução corrente em ContextMain, troca pelo ContextPong associado à função bodypong

   printf ("Main FIM\n");

   exit (0);
}
