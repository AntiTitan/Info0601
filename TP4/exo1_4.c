#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf, perror */
#include <unistd.h>    /* Pour fork, pipe */
#include <sys/wait.h>  /* Pour wait */
#include <sys/types.h> /* Pour pid_t */
#include <time.h>

int main(int argc, char * argv []){
    int n;
    
    if(argc!=2){
        printf("Pas assez d'arguments \n On attend un entier\n");
        return EXIT_FAILURE;
    }
    
    n=atoi(argv[1]);
    printf("Bonjour je vais attendre %d secondes\n",n);
    sleep(n);
    
    return EXIT_SUCCESS;
}