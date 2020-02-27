#include "f_voiture.h"

int recupererFile(int CLE_F) {
    int msqid;
    if((msqid = msgget((key_t)CLE_F, 0)) == -1) {
        perror("Erreur lors de la récupération de la file ");
        exit(EXIT_FAILURE);
    }
    return msqid;
}

int recupererMemoire(int CLE_M) {
    int shmid;
    if((shmid = shmget((key_t)CLE_M, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

int recupererSemaphores(int CLE_S) {
    int semid;
    if((semid = semget((key_t)CLE_S, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    return semid;
}

