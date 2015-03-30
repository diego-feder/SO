#include <unistd.h> // EXERCÍCIO 2 - PRÁTICA II - LAB SO
#include <stdio.h>  // DIEGO ASCÂNIO SANTOS
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[], char *envp[])
{
   int retval ;
   
   printf ("Ola, sou o processo %5d\n", getpid()) ;

   retval = fork () ;
   
   printf ("[retval: %5d] sou %5d, filho de %5d\n",
           retval, getpid(), getppid()) ;
             
   if ( retval < 0 )
   {
      perror ("Erro: ") ;
      exit (1) ;
   }
   else 
      if ( retval > 0 )
         wait (0) ;
      else
      {
         execve ("/bin/date", argv, envp) ; // a diferença desse programa
                // para o primeiro encontra-se nesse execve, pois nessa 
                // linha de código no outro programa (fork.c), havia
                // um comando sleep(5) que parava a execução do programa.
                // Porém, a syscall execve, chama um programa externo
                // e substitui o processo presente pelo programa externo
                // executado se o programa externo existir. Senão, uma men-
                // sagem de erro é lançada, a partir da função perror

                // O perror abaixo e o tchau do processo filho não são exe-
                // cutados quando o comando executado pelo comando execve
                // existe, uma vez que o processo filho terá seu contexto
                // substituido pelo programa executado pelo execve.
         perror ("Erro") ;
      }

   printf ("Tchau de %5d!\n", getpid()) ;

   exit (0) ;
}
