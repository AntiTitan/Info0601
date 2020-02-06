#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf, perror */
#include <unistd.h>    /* Pour fork, pipe */
#include <sys/wait.h>  /* Pour wait */
#include <sys/types.h> /* Pour pid_t */
#include <time.h>


int main(int argc, char * argv []){
    char * tab [3];

    if(argc!=3){
        printf("Pas assez d'arguments \n On attend deux entiers\n");
        return EXIT_FAILURE;
    }
    tab[0] =argv[1];
    tab[1]=argv[2];
    tab[2]= NULL;
    if(execve("./exo1_4",tab,NULL) == -1){
        perror("Erreur chargement ");
    }
    

    return EXIT_SUCCESS;
}