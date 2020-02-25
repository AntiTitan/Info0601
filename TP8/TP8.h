#ifndef _TP1_
#define _TP1_

#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <sys/sem.h>    /* Pour semget, semctl, semop */
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "ncurses.h"

#define NB_C    30      /* Nombre de colonnes matrice*/
#define NB_L    15      /* Nombre de lignes matrice*/

typedef struct {
    unsigned char grille[NB_L][NB_C];
} grille_t;

void afficheMsgFen(WINDOW*, char*);
void initZone(unsigned char mat[][NB_C]);
void Peux(int, int);
void Vas(int, int);
void afficheZone(unsigned char mat[][NB_C], WINDOW*, int);
WINDOW* creerFenetre(int, int, int, int);
WINDOW* creerSousFenetre(int, int, int, int, bool, WINDOW*);

#endif