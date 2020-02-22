#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include "TP8.h"

/**
 * Génère un entier aléatoire dans un intervalle [a, b].
 * @param a la borne minimale
 * @param b la borne maximale
 * @return l'entier aléatoire
 */
int alea(int a, int b) {
  return rand() % (b - a + 1) + a;
}


int main(int argc, char* argv[]) {
    int shmid, semid, i, CLE_S, CLE_M, c, l, color;
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

    while(1) {
        /* Modification de la grille du segment de mémoire partagée */
        srand(time(NULL)*getpid());
        c = alea(0, NB_C-1);
        l = alea(0, NB_L-1);
        if (c==0 && l==0) {               /*coin haut gauche*/
            Peux(NB_C*l+c, CLE_S);        /*case*/
            Peux(NB_C*l+c+1, CLE_S);      /*droite*/
            Peux(NB_C*(l+1)+c, CLE_S);    /*bas*/
            color = grille.grille[c][l];
            if (grille.grille[c][l+1] > color) color = grille.grille[c][l+1];
            if (grille.grille[c+1][l] > color) color = grille.grille[c+1][l];
            color++;
            grille.grille[c][l] = color % 5;
            grille.grille[c+1][l] = color % 5;
            grille.grille[c][l+1] = color % 5;
            Vas(NB_C*l+c, CLE_S);
            Vas(NB_C*l+c+1, CLE_S);
            Vas(NB_C*(l+1)+c, CLE_S);
        }
        else if (c==0 && l==NB_L-1) {         /*coin bas gauche*/
            Peux(NB_C*l+c, CLE_S);            /*case*/
            Peux(NB_C*(l-1)+c, CLE_S);        /*haut*/
            Peux(NB_C*l+c+1, CLE_S);          /*droite*/
            color = grille.grille[c][l];
            if (grille.grille[c][l-1] > color) color = grille.grille[c][l-1];
            if (grille.grille[c+1][l] > color) color = grille.grille[c+1][l];
            color++;
            grille.grille[c][l] = color % 5;
            grille.grille[c][l-1] = color % 5;
            grille.grille[c+1][l] = color % 5;
            Vas(NB_C*l, CLE_S);
            Vas(NB_C*(l-1), CLE_S);
            Vas(NB_C*l+1, CLE_S);
        }
        else if (c==NB_C-1 && l==NB_L-1) {/*coin bas droite*/
            Peux(NB_C*l+c, CLE_S);
            Peux(NB_C*(l-1)+c, CLE_S);
            Peux(NB_C*l+c-1, CLE_S);
            color = grille.grille[c][l];
            if (grille.grille[c][l-1] > color) color = grille.grille[c][l-1];
            if (grille.grille[c-1][l] > color) color = grille.grille[c-1][l];
            color++;
            grille.grille[c][l] = color % 5;
            grille.grille[c-1][l] = color % 5;
            grille.grille[c][l-1] = color % 5;
            Vas(NB_C*l+c, CLE_S);
            Vas(NB_C*(l-1)+c, CLE_S);
            Vas(NB_C*l+c-1, CLE_S);
        }
        else if (c==NB_C-1 && l==0) {/*coin haut droite*/
            Peux(NB_C*l+c, CLE_S);
            Peux(NB_C*l+c-1, CLE_S);
            Peux(NB_C*(l+1)+c, CLE_S);
            color = grille.grille[c][l];
            if (grille.grille[c][l+1] > color) color = grille.grille[c][l+1];
            if (grille.grille[c-1][l] > color) color = grille.grille[c-1][l];
            color++;
            grille.grille[c][l] = color % 5;
            grille.grille[c-1][l] = color % 5;
            grille.grille[c][l+1] = color % 5;
            Vas(NB_C*l+c, CLE_S);
            Vas(NB_C*l+c-1, CLE_S);
            Vas(NB_C*(l+1)+c, CLE_S);
        }
        else {
            Peux(NB_C*l+c, CLE_S);
            Peux(NB_C*l+c-1, CLE_S);/*gauche*/
            Peux(NB_C*l+c+1, CLE_S);/*droite*/
            Peux(NB_C*(l-1)+c, CLE_S);/*haut*/
            Peux(NB_C*(l+1)+c, CLE_S);/*bas*/
            color = grille.grille[c][l];
            if (grille.grille[c-1][l] > color) color = grille.grille[c-1][l];
            if (grille.grille[c+1][l] > color) color = grille.grille[c+1][l];
            if (grille.grille[c][l-1] > color) color = grille.grille[c][l-1];
            if (grille.grille[c][l+1] > color) color = grille.grille[c][l+1];
            color++;
            grille.grille[c][l] = color % 5;
            grille.grille[c-1][l] = color % 5;
            grille.grille[c+1][l] = color % 5;
            grille.grille[c][l-1] = color % 5;
            grille.grille[c][l+1] = color % 5;
            Vas(NB_C*l+c, CLE_S);
            Vas(NB_C*l+c-1, CLE_S);
            Vas(NB_C*l+c+1, CLE_S);
            Vas(NB_C*(l-1)+c, CLE_S);
            Vas(NB_C*(l+1)+c, CLE_S);

        }
    }



    /* Détachement du segment de mémoire partagée */
    if(shmdt(grille) == -1) {
        perror("Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
