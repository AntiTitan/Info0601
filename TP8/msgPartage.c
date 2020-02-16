#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include "struct.h"

int main() {
    int shmid, i;
    int *adresse;
    char message [CHAINE_MAX];
    pid_t pid;
    msg_t msg;

    /* Création d'un segment de 10 entiers */
    if((shmid = shmget((key_t)CLE, sizeof(int) * 10, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST)
        fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE);
    else
        perror("Erreur lors de la création du segment de mémoire ");
    exit(EXIT_FAILURE);
    }
    printf("Serveur : segment crée.\n");

    /* Attachement du segment de mémoire partagée */
    if((&msg = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }


    /* Placement du message dans le segment de mémoire partagée */
    printf("Message (%d max) : ", CHAINE_MAX);
    if(scanf("%20s",message)!=1){
        printf("Problème avec saisie du message\n");
        return EXIT_FAILURE;
    }
    msg.pid = pid;
    strcpy(msg.chaine, message);
    printf("Serveur : message placé dans le segment de memoire partagee.\n");

    /* Lecture du message du segment de mémoire partagée */
    printf("Client %d: msg: %s", msg.pid, msg.chaine);

    /* Détachement du segment de mémoire partagée */
    if(shmdt(&msg) == -1) {
        perror("Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
