#include <time.h>
#include "TP1.h"

#define LARGEURS  32                   /* Largeur de la fenêtre */
#define HAUTEURS  17                   /* Hauteur de la fenêtre */
#define LARGEUR_E 40                   /* Largeur de la fenêtre */
#define HAUTEUR_E 10                   /* Hauteur de la fenêtre */
#define LARGEUR LARGEURS + LARGEUR_E   /* Largeur de la fenêtre */
#define HAUTEUR 5                      /* Hauteur de la fenêtre */

#define POSX    0                      /* Position horizontale de la fenêtre */
#define POSY    3                      /* Position verticale de la fenêtre */
#define POSXS   0                      /* Position horizontale de la fenêtre */
#define POSYS   POSY + HAUTEUR + 1     /* Position verticale de la fenêtre */
#define POSX_E  POSXS + LARGEURS + 1   /* Position horizontale de la fenêtre */
#define POSY_E  POSY + HAUTEUR + 1     /* Position verticale de la fenêtre */



int main(){
    /* Matrice indique la présence de flocons (1) et d'obstacles (2) */
    unsigned char matrice[NB_L][NB_C];
    int i;
    WINDOW* fenetre, *simul, * etat,* sous_fenetre, * sous_simul, * sous_etat;
    int sourisX, sourisY;

    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs(); 

    /*Initialisation de la matrice*/
    /*initZoneAlea(matrice);*/
    initZone(matrice);
  
    /* Vérification des dimensions du terminal */
    if((COLS < POSX + LARGEUR) || (LINES < POSYS + HAUTEURS)) {
        ncurses_stopper();
        fprintf(stderr, 
              "Les dimensions du terminal sont insufisantes : l=%d,h=%d au lieu de l=%d,h=%d\n", 
              COLS, LINES, POSX + LARGEUR, POSY + HAUTEUR);
        exit(EXIT_FAILURE);
    }  
  
    /* Création de la fenêtre Info*/
    fenetre = creerFenetre(HAUTEUR, LARGEUR, POSY, POSX);
    sous_fenetre = creerSousFenetre(HAUTEUR - 2,LARGEUR - 2, POSY + 1, POSX + 1, TRUE, fenetre);

    /* Création de la fenêtre Simulation*/
    simul = creerFenetre(HAUTEURS, LARGEURS, POSYS, POSXS);
    sous_simul = creerSousFenetre(HAUTEURS - 2,LARGEURS - 2, POSYS + 1, POSXS + 1, FALSE, simul);
    
    /* Création de la fenêtre Etat*/
    etat = creerFenetre(HAUTEUR_E, LARGEUR_E, POSY_E, POSX_E);
    sous_etat = creerSousFenetre(HAUTEUR_E - 2,LARGEUR_E - 2, POSY_E + 1, POSX_E + 1, TRUE, etat);

    /* Definition de la palette */
    init_pair(1, COLOR_BLUE, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_BLUE, COLOR_MAGENTA);

    /* Colore le fond de la fenêtre */
    wbkgd(fenetre, COLOR_PAIR(1));
    wrefresh(fenetre);
    wbkgd(simul, COLOR_PAIR(2));
    wrefresh(simul);
    wbkgd(etat, COLOR_PAIR(3));
    wrefresh(etat);

    wbkgd(sous_fenetre, COLOR_PAIR(1));
    wrefresh(sous_fenetre);
    wbkgd(sous_simul, COLOR_PAIR(2));
    wrefresh(sous_simul);
    wbkgd(sous_etat, COLOR_PAIR(3));
    wrefresh(sous_etat);

    /* Ecriture des titres des fenêtres et des obstacles*/
    afficheMsgFen(fenetre, "   Infos ");
    afficheMsgFen(simul, "   Simulation ");
    afficheMsgFen(etat, "   Etat ");
    afficheZone(matrice,sous_simul);

    /* Gestion des flocons*/
    printw("Cliquez dans la fenetre ; pressez F2 pour quitter...");  
    /*timeout(200);*/
    while((i = getch()) != KEY_F(2)) {
        if((i == KEY_MOUSE) && (souris_getpos(&sourisX, &sourisY, NULL) == OK)) {
            if((sourisX > POSXS) && (sourisX < POSXS + LARGEURS - 1) &&
               (sourisY > POSYS) && (sourisY < POSYS + HAUTEURS - 1)) {
                creerObstacle(sourisX, sourisY, POSXS, POSYS, matrice);
                afficheZone(matrice,sous_simul);
            }
        }
    }

    /* Suppression des fenêtres et sous-fenêtres*/
    delwin(fenetre);
    delwin(simul);
    delwin(etat);
    delwin(sous_fenetre);
    delwin(sous_simul);
    delwin(sous_etat);
  
    /* Arrêt de ncurses */
    ncurses_stopper();


    return EXIT_FAILURE;
}