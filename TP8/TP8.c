#include <time.h>
#include "TP8.h"

/**
 * Affiche la chaine dans la fenetre.
 * @param win la fenetre dans laquelle on veut afficher le message
 * @param c le message que l'on veut afficher
 * @return void
 */
void afficheMsgFen(WINDOW* win,char* c) {
    wprintw(win, c);
    wrefresh(win);
}

/**
 * Initialise la matrice avec des 0.
 * @param mat la matrice a initialiser
 * @return void
 */
void initZone(unsigned char mat[][NB_C]) {
    int i,j;
    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            mat[i][j]=0;
        }
    }
}

/**
 * P(S) avec S un sémaphore.
 * @param sem   le sémaphore concerne
 * @param CLE   la cle IPC du tableau de semaphores
 * @return void
 */
void Peux(int sem, int CLE) {
    struct sembuf op;
    int semid;

    /* Récupération du tableau de sémaphores */
    if((semid = semget((key_t)CLE, 0, 0)) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    
    /* Réalisation de P(Sxx) */
    op.sem_num = sem;
    op.sem_op = -1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore ");
        exit(EXIT_FAILURE);
    }
}

/**
 * V(S) avec S un sémaphore.
 * @param sem   le sémaphore concerne
 * @param CLE   la cle IPC du tableau de semaphores
 * @return void
 */
void Vas(int sem, int CLE) {
    struct sembuf op;
    int semid;

    /* Récupération du tableau de sémaphores */
    if((semid = semget((key_t)CLE, 0, 0)) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    /* Réalisation de V(Sxx) */
    op.sem_num = sem;
    op.sem_op = 1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore ");
        exit(EXIT_FAILURE);
    }
    
}

/**
 * Affiche la grille dans une fenetre.
 * @param mat   la grille que l'on veut afficher
 * @param win   la fenetre dans laquelle on veut afficher la matrice
 * @param CLE   la cle IPC de la memoire partagee
 * @param semid l'id de la memoire partagee
 * @return void
 */
void afficheZone(unsigned char mat[][NB_C], WINDOW* win, int CLE) {
    int i,j,val, sem;
    
    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            sem = i*NB_C + j;
            Peux(sem, CLE);
            val = mat[i][j];
            
            switch (val) {
                case 0:
                    wattron(win, COLOR_PAIR(1));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(1));
                    break;
                case 1:
                    wattron(win, COLOR_PAIR(2));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(2));
                    break;
                case 2:
                    wattron(win, COLOR_PAIR(3));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(3));
                    break;
                case 3:
                    wattron(win, COLOR_PAIR(4));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(4));
                    break;
                case 4:
                    wattron(win, COLOR_PAIR(5));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(5));
                    break;
                case 5:
                    wattron(win, COLOR_PAIR(6));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(6));
                    break;
            }
            Vas(sem, CLE);
        }
    }
    wrefresh(win);
}

WINDOW* creerFenetre(int HAUTEUR, int LARGEUR, int POSY, int POSX) {
    WINDOW* fenetre;
    fenetre = newwin(HAUTEUR, LARGEUR, POSY, POSX);
    box(fenetre,0,0);
    return fenetre;
}

WINDOW* creerSousFenetre(int HAUTEUR, int LARGEUR, int POSY, int POSX, bool scroll, WINDOW* fenetre) {
    WINDOW* sous_fenetre;
    sous_fenetre = subwin(fenetre, HAUTEUR,LARGEUR, POSY, POSX);
    if (scroll == TRUE) scrollok(sous_fenetre, TRUE);
    return sous_fenetre;
}
