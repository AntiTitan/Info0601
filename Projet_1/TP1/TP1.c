#include <time.h>
#include "TP1.h"


void afficheMsgFen(WINDOW* win,char* c) {
    wprintw(win, c);
    wrefresh(win);
}

int updateFlocon(unsigned char mat[][NB_C], int * floc) {
    /*update la pos du flocons*/
    int x=floc[0], y=floc[1];

    if (x<NB_L-1 && mat[x+1][y]==0){
        floc[0]++;
        mat[x+1][y] = 1;
        mat[x][y] = 0;
        return 1;
    }
    else if (y > 0 && x<NB_L-1 && mat[x][y-1]==0 && mat[x+1][y-1]==0) {
        /* A gauche */
        floc[1]--;
        floc[0]++;
        mat[x+1][y-1] = 1;
        mat[x][y] = 0;
        return 1;
    }
    else if (y < NB_C-1 && x<NB_L-1 && mat[x][y+1]==0 && mat[x+1][y+1]==0) {
        /* A droite */
        floc[0]++;
        floc[1]++;
        mat[x+1][y+1] = 1;
        mat[x][y] = 0;
        return 1;
    }
    else {
        return 0;
    }
}

void initZone(unsigned char mat[][NB_C]) {
    int i,j;
    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            mat[i][j]=0;
        }
    }
}

void initZoneAlea(unsigned char mat[][NB_C]) {
    int i,j,nb;
    srand(time(NULL));
    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            nb = rand() % 15 + 1;
            if(nb==1){
                mat[i][j]=2;
            }
            else {
                mat[i][j]=0;
            }
        }
    }
    for(i=0;i<NB_C;i++){
        if(mat[0][i]==2){ mat[0][i]=0;}
    }
}

void afficheZone(unsigned char mat[][NB_C], WINDOW* win) {
    int i,j;

    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            if(mat[i][j]==1) {
                afficheMsgFen(win,"*");
            }
            else {
                if(mat[i][j]==2) {
                    wattron(win, COLOR_PAIR(4));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(4));
                }
                else {
                    afficheMsgFen(win," ");
                }
            }
        }
    }
    wrefresh(win);
}


void ajouterFlocon(unsigned char mat[][NB_C], int* nbf,int *floc){
    int x;
    x = rand() % NB_C + 1;

    floc[0]=0;floc[1]=x;
    mat[0][x]=1;

    (*nbf)++;
}

void creerObstacle(int sourisX, int sourisY, int POSX, int POSY, unsigned char mat[][NB_C]) {
    int x, y;

    x = sourisX-POSX;
    y = sourisY-POSY;

    mat[x][y] = 2; 
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
