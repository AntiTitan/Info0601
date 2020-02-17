#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include "struct.h"

#define NB_L 15
#define NB_C 30

int main(int argc, char* argv[]) {
    int shmid, i, j, CLE;
    unsigned char grille[NB_L][NB_C] ;

    if (argc != 3) {
        printf("Nombre d'arguments incorrect:  ");
    }

    CLE = atoi(argv[1]);

    /* Initialisation de la grille */
    for (i=; i<NB_L; i++) {
        for (j=0; j<NB_C; j++) {
            grille[i][j] = 0;
        }
    }

    /* Création d'un segment d'un grille de 15x30 message */
    if((shmid = shmget((key_t)CLE, sizeof(unsigned char)*NB_L*NB_C, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST) {
        fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE);
    }
    else
        perror("Erreur lors de la création du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur : segment mémoire partagée créé.\n");

    /* Attachement du segment de mémoire partagée */
    if((grille = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
/*A mettre dans une boucle*/
    /* Affichage de la grille du segment de mémoire partagée */
    for (i=; i<NB_L; i++) {
        for (j=0; j<NB_C; j++) {
            printf("%c ", grille[i][j]);
        }
        printf("\n");
    }

    /* Détachement du segment de mémoire partagée */
    if(shmdt(grille) == -1) {
        perror("Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
