#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <sys/sem.h>    /* Pour semget, semctl, semop */
#include <sys/msg.h>    /* Pour msgget, msgctl */
#include <string.h>   

int main(int argc, char * argv []){
    int cle,sh;

    if(argc == 1){
        printf("segment | semaphore | file\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1],"segment")){
        printf("Entrez cle seg memoire : ");
        if(scanf("%d",&cle)==0){
            fprintf(stderr,"Erreur lors du scanf");
            exit(EXIT_FAILURE);
        }
        /*recup et supression segment*/

        /* Récupération du segment de memoire partagee */
        if((sh = shmget((key_t)cle, 0, 0)) == -1) {
            fprintf(stderr, "Erreur lors de la récupération du segment de mémoire ");
            exit(EXIT_FAILURE);
        }
        /* Suppression du segment de mémoire partagée */
        if(shmctl(sh, IPC_RMID, 0) == -1) {
            fprintf(stderr, "Erreur lors de la suppression du segment de mémoire partagée ");
            exit(EXIT_FAILURE);
        }
    }
    if(strcmp(argv[1],"semaphore")){
        printf("Entrez cle semaphore : ");
        if(scanf("%d",&cle)==0){
            fprintf(stderr,"Erreur lors du scanf");
            exit(EXIT_FAILURE);
        }
        /*recup et supression segment*/

        /* Récupération du tableau de sémaphores */
        if((sh = semget((key_t)cle, 0, 0)) == -1) {
            fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
            exit(EXIT_FAILURE);
        }
        /* Suppression du tableau de sémaphores */
        if(semctl(sh, 0, IPC_RMID) == -1) {
            fprintf(stderr, "Erreur lors de la suppresion du tableau de sémaphores ");
            exit(EXIT_FAILURE);
        }
    }

    if(strcmp(argv[1],"file")){
        printf("Entrez cle file message : ");
        if(scanf("%d",&cle)==0){
            fprintf(stderr,"Erreur lors du scanf");
            exit(EXIT_FAILURE);
        }
        /*recup et supression segment*/

        /* Recuperation de la file */
        if((sh = msgget((key_t)cle, 0)) == -1) {
            if(errno == ENOENT)
            fprintf(stderr, "Aucune file présente avec la clé %d.\n", (key_t)cle);
            else
            perror("Erreur lors de la récupération de la file ");
            exit(EXIT_FAILURE);
        }
        /* Suppression de la file */
        if(msgctl(sh, IPC_RMID, 0) == -1) {
            perror("Erreur lors de la suppression de la file ");
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;

}

