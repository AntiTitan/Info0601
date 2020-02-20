#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include "TP8.h"


int main(int argc, char* argv[]) {
    int shmid, semid, i, CLE_S, CLE_M, c, l;
    grille_t * grille;

    if (argc != 3) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./client CLE_MEM, CLE_SEM\n");
        exit(EXIT_FAILURE);
    }

    if ((grille = malloc(sizeof(grille_t)))==NULL) {
        fprintf(stderr, "erreur malloc");
        exit(EXIT_FAILURE);
    }
    CLE_M = atoi(argv[1]);
    CLE_S = atoi(argv[2]);

    /* Récupération du tableau de sémaphores */
    if((semid = semget((key_t)CLE_S, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }

    /* Récupération du segment de memoire partagee */
    if((shmid = shmget((key_t)CLE_M, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    printf("Client : récupération du segment de mémoire partagée.\n");

    /* Attachement de la grille au segment de mémoire partagée */
    if((grille = shmat(shmid, NULL, 0)) == (void*)-1) {
        fprintf(stderr, "Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
/*A mettre dans une boucle*/
    /* Modification de la grille du segment de mémoire partagée */
    srand(time(NULL)*getpid());
    c = rand()%NB_C-1;
    l = rand()%NB_L-1;
    grille[c][l] = 'x';

    /* Détachement du segment de mémoire partagée */
    if(shmdt(grille) == -1) {
        perror("Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
