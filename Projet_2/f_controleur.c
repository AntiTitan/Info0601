#include "f_controleur.h"

int creerFile(int CLE_F) {
    int msqid;

    if((msqid = msgget((key_t)CLE_F, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Erreur : file (cle=%d) existante\n", CLE_F);
        else
            fprintf(stderr, "Erreur lors de la creation de la file ");
        exit(EXIT_FAILURE);
    }
    return msqid;
}

int creerMemoire(int CLE_M, int size) {
    int shmid;
    /* Création d'un segment d'un grille de 15x30 message */
    if((shmid = shmget((key_t)CLE_M, size, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        ncurses_stopper();
        if(errno == EEXIST) {
            fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE_M);
        }
        else
            fprintf(stderr, "Erreur lors de la création du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

void creerSemaphores(int CLE_S) {
    int semid;

    if((semid = semget((key_t)CLE_S, NBSEM, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE_S);
        else
            fprintf(stderr, "Erreur lors de la création du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    return semid;
}

void supprimerFile(int msqid) {
    if(msgctl(msqid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Erreur lors de la suppression de la file ");
        exit(EXIT_FAILURE);
    }

    printf("File supprimée.\n");
    return EXIT_SUCCESS;
}

void supprimerMemoire(int shmid) {
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Erreur lors de la suppression du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }

    printf("Memoire supprimée.\n");
    return EXIT_SUCCESS;
}

void supprimerSemaphores(int semid) {
    if(semctl(semid, 0, IPC_RMID) == -1) {
        fprintf(stderr, "Erreur lors de la suppresion du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }

    printf("Tableau de sémaphores supprimé.\n");
    return EXIT_SUCCESS;
}