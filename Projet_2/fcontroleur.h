#ifndef FCONTROLEUR_H
#define FCONTROLEUR_H

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ncurses.h"
#include "struct.h"

int creerFile(int);
int creerMemoire(int,int);
int creerSemaphores(int);

void supprimerFile(int msqid);
void supprimerMemoire(int shmid);
void supprimerSemaphores(int semid);

int ouvrir_fichier(char* nomfich);
int lire_fichier(int fic,unsigned char chaine [][NB_C],char * nom);
int fermer_fichier(int fic);

#endif

