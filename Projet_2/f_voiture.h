#ifndef F_VOITURES_H
#define F_VOITURES_H

#include "struct.h"

void handler_Voiture(int signal);

int alea(int a, int b);
int recupererFile(int CLE_F);
int recupererMemoire(int CLE_M);
int recupererSemaphores(int CLE_S);

#endif