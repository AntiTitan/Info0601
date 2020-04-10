#include "ncurses.h"

#include <ncurses.h>   /* Pour toutes les fonctions/constantes ncurses */
#include <stdlib.h>    /* Pour exit, EXIT_FAILURE */

/**
 * Initialisation de ncurses.
 */
void ncurses_initialiser() {
  initscr();	            /* Demarre le mode ncurses */
  cbreak();	              /* Pour les saisies clavier (desac. mise en buffer) */
  noecho();               /* Desactive l'affichage des caracteres saisis */
  keypad(stdscr, TRUE);	  /* Active les touches specifiques */
  refresh();              /* Met a jour l'affichage */
  curs_set(FALSE);        /* Masque le curseur */
}

/**
 * Fin de ncurses.
 */
void ncurses_stopper() {
  endwin();
}

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs() {
  /* Verification du support de la couleur */
  if(has_colors() == FALSE) {
    ncurses_stopper();
    fprintf(stderr, "Le terminal ne supporte pas les couleurs.\n");
    exit(EXIT_FAILURE);
  }

  /* Activation des couleurs */
  start_color();

  /* Definition de la palette */
  init_pair(0, COLOR_WHITE, COLOR_BLACK); /* pour les pneus */
  init_pair(1, COLOR_BLACK, COLOR_WHITE); /* pour les box */
  init_pair(2, COLOR_WHITE, COLOR_BLUE); /* pour l'etang */
  init_pair(3, COLOR_BLACK, COLOR_YELLOW); /* pour les poissons */
  init_pair(4, COLOR_BLACK, COLOR_GREEN); /* pour les requins */
	init_pair(5, COLOR_WHITE, COLOR_MAGENTA); /* pour les lignes */
  init_pair(6, COLOR_BLACK, COLOR_RED); /* pour les dynamites */

}

/**
 * Initialisation de la souris.
 */
void ncurses_souris() {
  if(!mousemask(ALL_MOUSE_EVENTS, NULL)) {
    ncurses_stopper();
    fprintf(stderr, "Erreur lors de l'initialisation de la souris.\n");
    exit(EXIT_FAILURE);
  }
 
  if(has_mouse() != TRUE) {
    ncurses_stopper();
    fprintf(stderr, "Aucune souris n'est détectée.\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Recupere la position x et y de la souris.
 * @param[out] x la position en x
 * @param[out] y la position en y
 * @param[out] bouton l'évenement associé au clic (ou NULL)
 * @return OK si reussite
 */
int souris_getpos(int *x, int *y, int *bouton) {
  MEVENT event;
  int resultat = getmouse(&event);
 
  if(resultat == OK) {
    *x = event.x;
    *y = event.y;
    if(bouton != NULL) *bouton = event.bstate;
  }
  return resultat;
}