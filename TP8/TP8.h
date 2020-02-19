#ifndef _TP1_
#define _TP1_

#include <stdlib.h>     /* Pour EXIT_FAILURE */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */

#include "ncurses.h"

#define NB_C    30                     /* Nombre de colonnes matrice*/
#define NB_L    15                     /* Nombre de lignes matrice*/

void afficheMsgFen(WINDOW*, char*);
void initZone(unsigned char mat[][NB_C]);
void afficheZone(unsigned char mat[][NB_C], WINDOW*);
WINDOW* creerFenetre(int, int, int, int);
WINDOW* creerSousFenetre(int, int, int, int, bool, WINDOW*);

#endif