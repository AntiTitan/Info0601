#ifndef F_CONTROLEUR_H
#define F_CONTROLEUR_H

#define _POSIX_C_SOURCE 199309L

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ncurses.h"
#include "struct.h"

int creerFile(int msqid);
int creerMemoire(int shmid,int size);
int creerSemaphores(int semid);

void supprimerFile(int msqid);
void supprimerMemoire(int shmid);
void supprimerSemaphores(int semid);

int ouvrir_fichier(char* nomfich);
int lire_fichier(int fic,unsigned char chaine [][NB_C],char * nom);
int fermer_fichier(int fic);

#endif

