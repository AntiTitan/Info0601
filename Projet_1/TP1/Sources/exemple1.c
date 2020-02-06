/**
 * Ce programme illustre le fonctionnement des fenetres ncurses.
 * Le programme cree une fenêtre, la colorie dans une couleur.
 * Pour quitter le programme, l'utilisateur doit cliquer dans la fenetre.
 * @author Cyril Rabat
 * @version 18/01/2017
 **/

#include <stdlib.h>     /* Pour EXIT_FAILURE */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */

#include "ncurses.h"

#define LARGEUR 20      /* Largeur de la fenêtre */
#define HAUTEUR 10      /* Hauteur de la fenêtre */
#define POSX    20      /* Position horizontale de la fenêtre */
#define POSY    5       /* Position verticale de la fenêtre */

int main() {
  int i;
  WINDOW* fenetre, * sous_fenetre;
  int quitter = FALSE;
  int sourisX, sourisY;

  /* Initialisation de ncurses */
  ncurses_initialiser();
  ncurses_souris();
  ncurses_couleurs(); 
  
  /* Vérification des dimensions du terminal */
  if((COLS < POSX + LARGEUR) || (LINES < POSY + HAUTEUR)) {
      ncurses_stopper();
      fprintf(stderr, 
              "Les dimensions du terminal sont insufisantes : l=%d,h=%d au lieu de l=%d,h=%d\n", 
              COLS, LINES, POSX + LARGEUR, POSY + HAUTEUR);
      exit(EXIT_FAILURE);
  }  
  
  /* Création de la fenêtre */
  fenetre = newwin(HAUTEUR, LARGEUR, POSY, POSX);
  box(fenetre,0,0);
  sous_fenetre = subwin(fenetre, HAUTEUR - 2,LARGEUR - 2, POSY + 1, POSX + 1);
  scrollok(sous_fenetre, TRUE);
  
  /* Colore le fond de la fenêtre */
  wbkgd(fenetre, COLOR_PAIR(2));
  wrefresh(fenetre);
  
  /* Attente du clic dans la fenêtre */
  printw("Cliquez dans la fenetre pour terminer...");
  while(quitter == FALSE) {
      i = getch();
      if((i == KEY_MOUSE) && 
         (souris_getpos(&sourisX, &sourisY, NULL) == OK))
            if((sourisX >= POSX) && (sourisX < POSX + LARGEUR) &&
               (sourisY >= POSY) && (sourisY < POSY + HAUTEUR))
                quitter = TRUE;
  }

  /* Suppression de la fenêtre */
  delwin(fenetre);
  
  /* Arrêt de ncurses */
  ncurses_stopper();

  return EXIT_FAILURE;
}