#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include "struct.h"

#define NB_L 15
#define NB_C 30

int main(int argc, char* argv[]) {
    int shmid, i, j, CLE, c, l;
    unsigned char grille[NB_L][NB_C] ;

    if (argc != 3) {
        printf("Nombre d'arguments incorrect:  ");
    }

    CLE = atoi(argv[1]);

    /* Récupération du segment d'un grille de 15x30 message */
    if((shmid = shmget((key_t)CLE, 0, 0)) == -1) {
        perror("Erreur lors de la récupération du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    printf("Client : récupération du segment de mémoire partagée.\n");

    /* Attachement du segment de mémoire partagée */
    if((grille = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de mémoire partagée ");
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
