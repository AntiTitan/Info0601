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

int supprimerFile(int msqid);
int supprimerMemoire(int shmid);
int supprimerSemaphores(int semid);

int ouvrir_fichier(char* nomfich);
int lire_fichier(int fic,unsigned char chaine [][NB_C],char * nom);
int fermer_fichier(int fic);

void afficheMsgFen(WINDOW* win,char* c);
void afficheZone(unsigned char mat[][NB_C], WINDOW* win);

WINDOW* creerFenetre(int, int, int, int);
WINDOW* creerSousFenetre(int, int, int, int, bool, WINDOW*);

#endif

