#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h> 
#include <string.h>
#include "struct.h"

int main(int argc, char* argv[]) {
    int shmid, CLE;
    char message [CHAINE_MAX];
    pid_t pid;
    msg_t* msg;

    if (argc != 3) {
        printf("Nombre d'arguments incorrect: ./msgPartage  CLE_IPC message\n");
        exit(EXIT_FAILURE);
    }

    if ((msg = malloc(sizeof(msg_t)))==NULL) {
        perror("erreur malloc");
        exit(EXIT_FAILURE);
    }
    CLE = atoi(argv[1]); 
    strcpy(message, argv[2]);

    /* Création d'un segment d'un message */
    if((shmid = shmget((key_t)CLE, sizeof(msg_t), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST) {
            fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE);

            /* Récupération du segment de mémoire partagée */
            if((shmid = shmget((key_t)CLE, 0, 0)) == -1) {
                perror("Erreur lors de la récupération du segment de mémoire partagée ");
                exit(EXIT_FAILURE);
            }
            
            printf("Client : récupération du segment de mémoire partagée.\n");
        }
        else{
            perror("Erreur lors de la création du segment de mémoire ");
            exit(EXIT_FAILURE);
        }
    }
    printf("Serveur : segment mémoire partagée créé.\n");

    /* Attachement du segment de mémoire partagée */
    if((msg = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
    printf("message precedent : %d\t%s\n",msg->pid, msg->chaine);

    /* Placement du message dans le segment de mémoire partagée */
    pid = getpid();
    msg->pid = pid;
    strcpy(msg->chaine, message);
    printf("Serveur : message placé dans le segment de memoire partagee.\n");

    /* Lecture du message du segment de mémoire partagée */
    printf("Client %d: msg: %s\n", msg->pid, msg->chaine);

    /* Détachement du segment de mémoire partagée */
    if(shmdt(msg) == -1) {
        perror("Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
