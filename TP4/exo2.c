/**
 * Ce programme montre comment créer un tube anonyme entre un processus père
 * et un processus fils. Il est ensuite utilisé pour envoyer 5 entiers du
 * père vers le fils.
 * @author Cyril Rabat
 * @version 21/01/2020
 */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf, perror */
#include <unistd.h>    /* Pour fork, pipe */
#include <sys/wait.h>  /* Pour wait */
#include <sys/types.h> /* Pour pid_t */
#include <string.h>
	

void viderBuffer(){
    int c = 0;
    while (c != '\n' && c != EOF){
        c = getchar();
    }
}

void fils(int* tube, int n) {
    int nb;
    char chaine [31];
    /*char msg[20];*/

    if((nb=read(tube[0], chaine, sizeof(char)*30)) == -1) {
        perror("Fils : erreur lors de la lecture depuis le tube ");
        exit(EXIT_FAILURE);
    }
    chaine[nb] = '\0';
    /*sprintf(msg, "%d bien recu", n);
    if(write(tube[1], msg, sizeof(char)*strlen(msg)) == -1) {
        perror("Père : erreur lors de l'écriture dans le tube ");
        exit(EXIT_FAILURE);
    }*/
    while(strcmp(chaine,"FINPROG")!=0){
        printf("Fils %d : chaine lu : %s\n", n+1, chaine);
        if((nb=read(tube[0], chaine, sizeof(char)*30)) == -1) {
            perror("Fils : erreur lors de la lecture depuis le tube ");
            exit(EXIT_FAILURE);
        }
        chaine[nb] = '\0';
    }

    /* Fermeture du tube en écriture */
    if(close(tube[1]) == -1) {
        perror("Fils : erreur lors de la fermeture du tube en écriture ");
        exit(EXIT_FAILURE);
    }

    /* Fermeture du tube en lecture */
    if(close(tube[0]) == -1) {
        perror("Fils : erreur lors de la fermeture du tube en lecture ");
        exit(EXIT_FAILURE);
    }

    printf("Fils %d : terminé.\n", n);
    
    exit(EXIT_SUCCESS);
}

int main(int argc, char * argv[]) {
    pid_t* pid;
    int **tube, i, nb=0,r=0,n=0,retour;
    char chaine [30];

    if(argc!=2){
        printf("Pas assez d'arguments \n On attend un entier\n");
        return EXIT_FAILURE;
    }
    
    n=atoi(argv[1]);
    tube = malloc(sizeof(int*)*n);
    pid = malloc(sizeof(pid_t)*n);

    /* Création des tube */
    for(i=0; i<n; i++) {
        tube[i] = malloc(sizeof(int)*2);
        if(pipe(tube[i]) == -1) {
            perror("Père : erreur lors de la création du tube ");
            exit(EXIT_FAILURE);
        }
    }

    /* Création des fils */
    for(i=0;i<n;i++){
        if((pid[i] = fork()) == -1) {
            perror("Père : erreur lors de la création du fils ");
            exit(EXIT_FAILURE);
        }
        if(pid[i] == 0){
            fils(tube[i],i);
        }
    }

    /*Saisie au clavier*/
    while(nb!=-1) {
        while ((nb!=-1) && (nb<1 || nb>n)){
            printf("Entrez l'entier entre 1 et %d: ",n);
            if(scanf("%d",&nb)!=1){
                perror("Pb scanf :");
                exit(EXIT_FAILURE);
            }
        }
        /*Vidage du buffer*/
        viderBuffer();

        if(nb!=-1){
            printf("Entrez la chaine de cara: ");
            if(fgets(chaine, sizeof(chaine), stdin)==NULL){
                perror("Pb fgets :");
                exit(EXIT_FAILURE);
            }
            
            /* Envoi de la chaine au fils */
            if(write(tube[nb-1][1], chaine, sizeof(char)*strlen(chaine)) == -1) {
                perror("Père : erreur lors de l'écriture d'un entier dans le tube ");
                exit(EXIT_FAILURE);
            }
            printf("Père : chaine envoyée au fils %d\n", nb);
            sleep(1);
            nb=0;
        }
        /* Reception du msg "X bien recu "*/
    }

    /* Fermeture du tube en lecture */
    for(i=0;i<n;i++) {
        if(close(tube[i][0]) == -1) {
            perror("Père : erreur lors de la fermeture du tube en lecture ");
            exit(EXIT_FAILURE);
        }
    }

   
    /*Envoie code de fin aux fils*/
    for(i=0; i<n; i++) {
        printf("Père : envoie FINPROG au fils %d\n", i);
        if(write(tube[i][1], "FINPROG", sizeof(char)*strlen("FINPROG")) == -1) {
            perror("Père : erreur lors envoi message de fin dans le tube ");
            exit(EXIT_FAILURE);
        }
        sleep(2);
    }
    /* Fermeture du tube en écriture */
    for(i=0; i<n; i++) {
        if(close(tube[i][1]) == -1) {
            perror("Père : erreur lors de la fermeture du tube en ecriture ");
            exit(EXIT_FAILURE);
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
