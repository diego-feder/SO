#include <unistd.h> // EXERCÍCIO 1 - PRÁTICA II - LAB SO
#include <stdio.h>  // DIEGO ASCÂNIO SANTOS
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main () {
    int retval; // variavel para receber o pid de retorno do comando fork

    /* esse comando é somente executado pelo processo pai
       e ele imprime o pid atribuido a esse processo pelo
       sistema operacional */
    printf ("Ola, sou o processo %5d\n", getpid()); 

    retval = fork(); // o comando fork cria um processo filho que é uma
                     // cópia exata do pai, à exceção do seu pid. O va-
                     // lor de return de fork é de pid do filho para 
                     // a execução do processo pai e de 0 para a execu-
                     // ção do processo filho

    printf ("[retval: %5d] sou %5d, filho de %5d\n",
            retval, getpid(), getppid()); // esse comando sera executado
                                          // pelos dois processos, tanto
                                          // o pai quanto o filho
            // No processo pai, mostra-se o retval contendo o pid do fi-
            // lho, o seu pid e pid de seu pai, normalmente o pid do sh-
            // ell que o executa. No filho, mostra retval = 0, pois esse
            // é o valor que fork retorna no filho, o pid do filho e o 
            // pid de seu pai

    if (retval < 0) { // quando o fork falha, a função retorna -1
                      // o que consiste em erro
        perror ("Erro");
        exit (1);
    }
    else {
        if (retval > 0) { // quando o retval é maior que 0, é sinal
                          // de que o processo em execução é o pro
                          // cesso pai; no comando wait(0), o pai 
                          // espera o término da execução do filho
            wait(0);
        }
        else {
            sleep(5); // quando retval = 0, o processo em execução
                      // é o filho e esse comando sleep(5) para o
                      // programa por 5 segundos.
        }
    }

    printf ("Tchau de %5d!\n", getpid()); // No final os dois pro-
           // cessos executam esse comando, mostrando os próprios
           // pids. Primeiro o processo filho, logo em seguida, o 
           // pai, após esperar o término do filho

    exit (0);
}
