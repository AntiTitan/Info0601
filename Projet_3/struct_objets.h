#ifndef STRUCT_OBJETS_H
#define STRUCT_OBJETS_H

/*include */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

/*define */

#define VIDE 0
#define POISSON 1
#define REQUIN 2
#define DYNAMITE 3
#define PNEU 4
#define LIGNE 5

#define POISSON1 1
#define POISSON2 2
#define POISSON3 3

/*struct*/

    /*les differentes choses que l'on peut trouver dans l'étang*/
typedef struct{
    int typeObjet=0;
    int idPoiscaille;
    int typePoisson;
    int idJoueur;
    pthread_t threadPoisson; /* un thread spécifique pour le requin ?*/
    pthread_t threadChrono; 
    /*besoin d'un thread choro, pour gérer la ligne, la fuite, le mode furtif ?*/
}objet_t;

    /* grille représentant l'étang */
typedef struct{
    int largeur;
    int hauteur;
    objet_t **  objet;
    /*mutex et variable cond*/
}grille_t;

typedef struct{
    int idJoueur;
    int nbPoints;
    int poireaus;
    /*mutex et variable cond*/
}

#endif