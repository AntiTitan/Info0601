#ifndef F_CONTROLEUR_H
#define F_CONTROLEUR_H

#include <sys/msg.h>    /* Pour msgget, msgctl */
#include <sys/sem.h>    /* Pour semget, semctl */
#include <sys/shm.h>    /* Pour shmget, shmctl */
#include <errno.h>      /* Pour errno */

#include "ncurses.h"
#include "struct.h"

#define TYPE_RECUP_CONFIG 1
#define TYPE_ENVOI_CONFIG 2
#define TYPE_MODIF_CARTE  3

#define NB_C    30      /* Nombre de colonnes matrice*/
#define NB_L    15      /* Nombre de lignes matrice*/
#define NBSEM    NB_C*NB_L

typedef struct {
    unsigned char grille[NB_L][NB_C];
} grille_t;

void creerFile(int msqid);
void creerMemoire(int shmid);
void creerSemaphores(int semid);

void supprimerFile(int msqid);
void supprimerMemoire(int shmid);
void supprimerSemaphores(int semid);

#endif