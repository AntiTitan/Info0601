#define _XOPEN_SOURCE
#include "fonctions_sys.h"


int creerSemaphores(int CLE_S) {
    int semid;

    if((semid = semget((key_t)CLE_S, MAX_PARTIE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE_S);
        else
            fprintf(stderr, "Erreur lors de la création du tableau de sémaphores \n");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int supprimerSemaphores(int semid) {
    if(semctl(semid, 0, IPC_RMID) == -1) {
        fprintf(stderr, "Erreur lors de la suppresion du tableau de sémaphores Controleur \n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdin, "Tableau de sémaphores supprimé.\n");
    return EXIT_SUCCESS;
}

int recupererSemaphores(int CLE_S) {
    int semid;
    if((semid = semget((key_t)CLE_S, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphoresss \n");
        exit(EXIT_FAILURE);
    }
    return semid;
}

/**
 * P(S) avec S un sémaphore.
 * @param sem     le sémaphore concerne
 * @param semid   le tableau de semaphores
 * @return void
 */
void Peux(int sem, int semid) {
    struct sembuf op;

    /* Réalisation de P(Sxx) */
    op.sem_num = sem;
    op.sem_op = -1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore Peux(%d)\n", sem);
        exit(EXIT_FAILURE);
    }
}

/**
 * V(S) avec S un sémaphore.
 * @param sem     le sémaphore concerne
 * @param semid   le tableau de semaphore
 * @return void
 */
void Vas(int sem, int semid) {
    struct sembuf op;

    /* Réalisation de V(Sxx) */
    op.sem_num = sem;
    op.sem_op = 1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore Vas(%d)\n", sem);
        exit(EXIT_FAILURE);
    }
    
}