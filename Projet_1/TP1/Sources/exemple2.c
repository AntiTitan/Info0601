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
  WINDOW* fenetre;
  WINDOW* sous_fenetre;
  int sourisX, sourisY, cpt = 0;

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
  
  /* Creation de la fenêtre */
  fenetre = newwin(HAUTEUR, LARGEUR, POSY, POSX);
  box(fenetre, 0, 0);
  sous_fenetre = subwin(fenetre, HAUTEUR - 2, LARGEUR - 2, POSY + 1, POSX + 1);
  scrollok(sous_fenetre, TRUE);
  
  /* Création d'un cadre */
  wrefresh(fenetre);
  wrefresh(sous_fenetre);
  
  /* Attente d'un clic dans la fenêtre ou de F2 */
  printw("Cliquez dans la fenetre ; pressez F2 pour quitter...");  
  while((i = getch()) != KEY_F(2)) {
      if((i == KEY_MOUSE) && 
         (souris_getpos(&sourisX, &sourisY, NULL) == OK))
            if((sourisX > POSX) && (sourisX < POSX + LARGEUR - 1) &&
               (sourisY > POSY) && (sourisY < POSY + HAUTEUR - 1)) {
                cpt++;
                if(cpt != 1) wprintw(sous_fenetre, "\n");
                wprintw(sous_fenetre, "Ca chatouille %d", cpt);
                wrefresh(sous_fenetre);
            }
  }

  /* Suppression des fenêtres */
  delwin(sous_fenetre);
  delwin(fenetre);
  
  /* Arrêt de ncurses */
  ncurses_stopper();

  return EXIT_FAILURE;
}