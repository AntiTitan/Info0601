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

/*struct*/

/* case */
typedef struct{
    void * objet=NULL; /* (void *) ou struct objet qui peut être n'importe lequel des objets*/
}case_t;

/* poisson */
typedef struct{
    int type;
    pthread_t threadpoisson;
}poisson_t;

/* dynamite */
typedef struct{
    int idJoueur;
}dynamite_t;

/* requin */
typedef struct{
    int type;
    int idJoueur;
    pthread_t threadrequin; /*ou threadpoisson*/
}requin_t;

/* pneu */
typedef struct{
    int idJoueur;
    
}pneu_t;

/* ligne */
typedef struct{
    int type;
    pthread_t threadchrono; /*ou pas de thread chrono*/
}ligne_t;

/******** ou alors on garde les cases ************/
#define VIDE 0
#define POISSON 1
#define REQUIN 2
#define DYNAMITE 3
#define PNEU 4
#define LIGNE 5

#define POISSON1 1
#define POISSON2 2
#define POISSON3 3


typedef struct{
    int typeObjet=0;
    int idPoiscaille;
    int typePoisson;
    int idJoueur;
    pthread_t threadPoisson;
    pthread_t threadChrono;
}objet_t;

/* case */
typedef struct{
    objet_t objet;
}caseAlternative_t;

#endif