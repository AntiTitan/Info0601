#define _XOPEN_SOURCE
#ifndef STRUCT_OBJETS_H
#define STRUCT_OBJETS_H

/*include */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

/* define */

#define VIDE      0
#define POISSON   1
#define REQUIN    2 
#define DYNAMITE  3
#define PNEU      4
#define LIGNE     5

#define POISSON1  1
#define POISSON2  2
#define POISSON3  3


/* structures */

    /* les differentes choses que l'on peut trouver dans l'étang */
typedef struct obj{
    int typeObjet;
    int idPoiss;
    int typePoisson;
    int idJoueur;
    int idJDeux; /* au cas où deux joueurs posent leur ligne au même endroit*/
    int position [2];
    pthread_t threadPoisson; /* à ne pas envoyer */
    pthread_t threadChrono;  /* à ne pas envoyer */
    pthread_mutex_t mutObj;  /* à ne pas envoyer */
    pthread_cond_t contObj;  /* à ne pas envoyer */
} objet_t;

    /* grille représentant l'étang */
typedef struct grille{
    int largeur;
    int hauteur;
    objet_t **  objet;
} grille_t;

    /* structure qui résume les informations d'un joueur */
typedef struct j{
    int idJoueur;
    int nbPoints;
    int poireaus;
    pthread_mutex_t mutJoueur;
    /*mutex et variable cond*/
} joueur_t;

#endif