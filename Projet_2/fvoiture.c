#define _XOPEN_SOURCE
#include "fvoiture.h"

/**
 * Génère un entier aléatoire dans un intervalle [a, b].
 * @param a la borne minimale
 * @param b la borne maximale
 * @return l'entier aléatoire
 */
int alea(int a, int b) {
    int al;
    al = rand() % (b - a + 1) + a;
    return (al);
}

int recupererFile(int CLE_F) {
    int msqid;
    if((msqid = msgget((key_t)CLE_F, 0)) == -1) {
        perror("Erreur lors de la récupération de la file \n");
        exit(EXIT_FAILURE);
    }
    return msqid;
}

int recupererMemoire(int CLE_M) {
    int shmid;
    if((shmid = shmget((key_t)CLE_M, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du segment de mémoire \n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

int recupererSemaphores(int CLE_S) {
    int semid;
    if((semid = semget((key_t)CLE_S, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphoresss \n");
        exit(EXIT_FAILURE);
    }
    return semid;
}

