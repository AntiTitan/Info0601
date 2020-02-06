#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf, perror */
#include <unistd.h>    /* Pour fork, pipe */
#include <sys/wait.h>  /* Pour wait */
#include <sys/types.h> /* Pour pid_t */
#include <time.h>

#define MAX 1000

void fils(int n) {
    int a;
    srand(time(NULL)*n*getpid());
    a=rand()%MAX;
    printf("Fils %d : nombre = %d\n",n,a);
    printf("Fils : terminé.\n");
    a=rand()%10+1;
    sleep(a);
    exit(n);
}

int main(int argc, char * argv []){
    int n,i,retour,r;
    pid_t * pid;

    if(argc!=2){
        printf("Pas assez d'arguments \n On attend un entier\n");
        return EXIT_FAILURE;
    }
    
    n=atoi(argv[1]);
    pid = malloc(sizeof(pid_t)*n);
    /* Création des fils */
    for(i=0;i<n;i++){    
        if((pid[i] = fork()) == -1) {
            perror("Père : erreur lors de la création du fils ");
            exit(EXIT_FAILURE);
        }
        if(pid[i] == 0){
            fils(i);
        }
    }

    /* Attente de la fin des fils */
    for(i=0;i<n;i++){
        if((r=wait(&retour)) == -1) {
            perror("Père : erreur lors de l'attente de la fin du fils ");
            exit(EXIT_FAILURE);
        }
        if(WIFEXITED(retour)){
            printf("Fils %d terminé, valeur renvoyée : %d\n",r,WEXITSTATUS(retour));
        }
        else{
            printf("Fils %d terminé anormalement\n",r);
        }
    }
    printf("Père : fils terminés.\n");

    return EXIT_SUCCESS;
}