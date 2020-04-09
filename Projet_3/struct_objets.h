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

#define POISSON1  1
#define POISSON2  2
#define POISSON3  3

#define LIBRE     0
#define BOUGE     1
#define AFFICHE   2
#define PECHE     3
#define POSE      4

#define HAUT 0
#define BAS 2
#define DROITE 1
#define GAUCHE 3


/* structures */

    /* les differentes choses que l'on peut trouver dans l'étang */
typedef struct obj{
    int typeObjet;
    int idPoiss;
    int typePoisson;
    int idJoueur;
    int idLigne1; /* au cas où deux joueurs posent leur ligne au même endroit*/
    /*chez le joueur, sa ligne seulement sera indiquée (en idLigne1)*/
    int idLigne2; /* au cas où deux joueurs posent leur ligne au même endroit*/
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
    /* structre de coordonnées */
typedef struct c {
	int x,y,partie;
} coord_t;

/* structre de poisson pour le joueur */
typedef struct c {
	int x,y,id,type;
} poisson_t;

#endif