#include "struct.h"


/**
 * P(S) avec S un sémaphore.
 * @param sem   le sémaphore concerne
 * @param CLE   la cle IPC du tableau de semaphores
 * @return void
 */
void Peux(int sem, int CLE) {
    struct sembuf op;
    int semid;

    /* Récupération du tableau de sémaphores */
    if((semid = semget((key_t)CLE, 0, 0)) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    /* Réalisation de P(Sxx) */
    op.sem_num = sem;
    op.sem_op = -1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore ");
        exit(EXIT_FAILURE);
    }
}

/**
 * V(S) avec S un sémaphore.
 * @param sem   le sémaphore concerne
 * @param CLE   la cle IPC du tableau de semaphores
 * @return void
 */
void Vas(int sem, int CLE ) {
    struct sembuf op;
    int semid;

    /* Récupération du tableau de sémaphores */
    if((semid = semget((key_t)CLE, 0, 0)) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    /* Réalisation de V(Sxx) */
    op.sem_num = sem;
    op.sem_op = 1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore ");
        exit(EXIT_FAILURE);
    }
    
}