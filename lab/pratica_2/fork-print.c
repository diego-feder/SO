#include <unistd.h> // EXERCÍCIO 3 - PRÁTICA II - LAB SO
#include <stdio.h>  // DIEGO ASCÂNIO SANTOS
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ()
{
   int retval, x ;

   x = 0 ;
   
   retval = fork () ;

   printf ("No processo %5d x vale %d\n", getpid(), x) ;
   
   if ( retval < 0 )
   {
      perror ("Erro") ;
      exit (1) ;
   }
   else
      if ( retval > 0 )
      {
         x = 0 ;
         wait (0) ;
      }
      else
      {
         x++ ; // No processo filho, que recebe uma cópia identica do pai
               // com as mesmas variáveis, o valor da variável x é incre-
               // mentado e o programa filho para sua execução por 5 seg.
               // No final das execuções, somente no processo filho que
               // a variável x vai valer 1, uma vez que ele tem uma cópia
               // dessa variável e não um link para ela, pois os processos
               // não a compartilham.
         sleep (5) ;
      }

   printf ("No processo %5d x vale %d\n", getpid(), x) ;

   exit (0) ;
}
