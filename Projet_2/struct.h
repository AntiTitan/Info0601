#define _XOPEN_SOURCE
#ifndef STRUCT_H
#define STRUCT_H


#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv, msgctl */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h> 
#include <sys/types.h>
#include <signal.h>    /* Pour sigaction */
#include <sys/sem.h>    /* Pour semget, semctl */
#include <sys/shm.h>    /* Pour shmget, shmctl */

#include "ncurses.h"

#define TYPE_RECUP_CONFIG 1
#define TYPE_ENVOI_CONFIG 2
#define TYPE_MODIF_CARTE  3

#define VIDE 0
#define ROUTE 1

#define MAX_VOITURE       10

#define NB_C     30     /* Nombre de colonnes matrice*/
#define NB_L     15     /* Nombre de lignes matrice*/
#define LARGEUR  32     /* Largeur de la fenêtre */
#define HAUTEUR  17     /* Hauteur de la fenêtre */
#define POSX     0      /* Position horizontale de la fenêtre */
#define POSY     3      /* Position verticale de la fenêtre */

/*#define NBSEM    NB_C*NB_L/25*/
#define NBSEM    1

typedef struct {
    unsigned char grille[NB_L][NB_C];
} grille_t;

typedef struct {
    unsigned char grille[5][5];
} grille5_t;

typedef struct info {
    grille_t carte;
    int position [MAX_VOITURE][2];
} info_t;

typedef struct rconfig {
    long type;
    pid_t pid; /*pid demandeur des clé du seg memoire et tableau séaphore*/
} r_config_t;

typedef struct econfig {
    long type;
    key_t cle_mem;
    key_t cle_sema;
} e_config_t;

typedef struct mcarte {
    long type;
    /*un message ?*/
    int voiture;
} modif_carte_t;

void Peux(int sem, int CLE);
void Vas(int sem, int CLE );

#endif