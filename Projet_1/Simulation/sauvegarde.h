#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>

#define NB_C    30                     /* Nombre de colonnes matrice*/
#define NB_L    15                     /* Nombre de lignes matrice*/
#define FLOC    2                      /* Constante pour coordonnées du flocon en cours*/
#define MAX_NOM    20                     /* taille maximale du nom du fichier*/

int ouvrir_fichier(char*);
int lire_fichier_d(int,unsigned char[],char*);
int lire_fichier_s(int,unsigned char[],char*);
int afficher_fichier(int);
int ecrire_fichier_d(int,unsigned char[][NB_C],char *);
int ecrire_fichier_s(int,unsigned char[][NB_C],int[FLOC],char *);
int fermer_fichier(int);
void ecrire_matrice(unsigned char[][NB_C],unsigned char[],char*);
void ecrire_flocon(int[FLOC],unsigned char[],char*);

#endif