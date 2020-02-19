#include <time.h>
#include "TP1.h"


void afficheMsgFen(WINDOW* win,char* c) {
    wprintw(win, c);
    wrefresh(win);
}

void initZone(unsigned char mat[][NB_C]) {
    int i,j;
    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            mat[i][j]=0;
        }
    }
}

void afficheZone(unsigned char mat[][NB_C], WINDOW* win) {
    int i,j,val, sem;

    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            sem = i*NB_C + j;
            val = mat[i][j];
            /* Réalisation de P(Sxx) */
            op.sem_num = sem;
            op.sem_op = -1;
            op.sem_flg = 0;
            if(semop(semid, &op, 1) == -1) {
                perror("Erreur lors de l'opération sur le sémaphore ");
                exit(EXIT_FAILURE);
            }
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
            /* Réalisation de V(Sxx) */
            op.sem_num = sem;
            op.sem_op = 1;
            op.sem_flg = 0;
            if(semop(semid, &op, 1) == -1) {
                perror("Erreur lors de l'opération sur le sémaphore ");
                exit(EXIT_FAILURE);
            }
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
