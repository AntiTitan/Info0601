#ifndef F_VOITURES_H
#define F_VOITURES_H

#include "struct.h"

int recupFile(int msqid);
int recupMemoire(int CLE_Me);
int recupSemaphores(int semid);

int alea(int a, int b);
int recupererFile(int CLE_F);
int recupererMemoire(int CLE_M);
int recupererSemaphores(int CLE_S);

#endif