#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <sys/sem.h>    /* Pour semget, semctl, semop */
#include "struct.h"
#include "TP8.h"

#define LARGEUR  32     /* Largeur de la fenêtre */
#define HAUTEUR  17     /* Hauteur de la fenêtre */
#define POSX     0      /* Position horizontale de la fenêtre */
#define POSY     3      /* Position verticale de la fenêtre */
#define NBSEM    NB_C*NB_L

int main(int argc, char* argv[]) {
    int shmid, semid, i, CLE;
    unsigned short val[NBSEM];/*tableau d'initialisation des sem*/
    unsigned char *grille;
    WINDOW* fenetre, * sous_fenetre;

    if (argc != 3) {
        printf("Nombre d'arguments incorrect:  ");
        exit(EXIT_FAILURE);
    }

    CLE = atoi(argv[1]);

    /* Création du tableau de sémaphores */
    if((semid = semget((key_t)CLE, NBSEM, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE);
        else
            perror("Erreur lors de la création du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }

    /* Initialisation du tab d'initialisation des sem*/
    for (i=0; i<NBSEM; i++) {
        val[i] = 0;
    }

    /* Initialisation des sémaphores */
    if(semctl(semid, 0, SETALL, val) == -1) {
        perror("Erreur lors de l'initialisation des sémaphores ");
        exit(EXIT_FAILURE);
    }

    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();

    /* Vérification des dimensions du terminal */
    if((COLS < POSX + LARGEUR) || (LINES < POSY + HAUTEUR)) {
        ncurses_stopper();
        fprintf(stderr, 
            "Les dimensions du terminal sont insufisantes : l=%d,h=%d au lieu de l=%d,h=%d\n", 
            COLS, LINES, POSX + LARGEUR, POSY + HAUTEUR);
        exit(EXIT_FAILURE);
    }  

    /* Création de la fenêtre d'affichage*/
    fenetre = creerFenetre(HAUTEUR, LARGEUR, POSY, POSX);
    sous_fenetre = creerSousFenetre(HAUTEUR - 2,LARGEUR - 2, POSY + 1, POSX + 1, FALSE, fenetre);


    /* Initialisation de la grille */
    initZone(grille);

    /* Création d'un segment d'un grille de 15x30 message */
    if((shmid = shmget((key_t)CLE, sizeof(grille), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
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

    timeout(500);
    while((i = getch()) != KEY_F(2)) {
        /* Affichage de la grille du segment de mémoire partagée */
        afficheZone(grille, sous_fenetre);
    }

    /* Détachement du segment de mémoire partagée */
    if(shmdt(grille) == -1) {
        perror("Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
/*

   Réalisation de P(Sxx)
  printf("Fils1 : attente du sémaphore S2 -> P(S2)\n");
  op.sem_num = xx;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

   Réalisation de V(Sxx)
  printf("Fils1 : libération du semaphore S2 -> V(S2)\n");
  op.sem_num = xx;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

*/