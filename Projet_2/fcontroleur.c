#define _XOPEN_SOURCE
#include "fcontroleur.h"

int creerFile(int CLE_F) {
    int msqid;

    if((msqid = msgget((key_t)CLE_F, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        ncurses_stopper();
        if(errno == EEXIST)
            fprintf(stderr, "Erreur : file (cle=%d) existante\n", CLE_F);
        else
            fprintf(stderr, "Erreur lors de la creation de la file \n");
        exit(EXIT_FAILURE);
    }
    return msqid;
}

int creerMemoire(int CLE_M, int size) {
    int shmid;
    /* Création d'un segment d'un grille de 15x30 message */
    if((shmid = shmget((key_t)CLE_M, size, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        ncurses_stopper();
        if(errno == EEXIST) {
            fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE_M);
        }
        else
            fprintf(stderr, "Erreur lors de la création du segment de mémoire \n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

int creerSemaphores(int CLE_S) {
    int semid;

    if((semid = semget((key_t)CLE_S, NBSEM, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        ncurses_stopper();
        if(errno == EEXIST)
            fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE_S);
        else
            fprintf(stderr, "Erreur lors de la création du tableau de sémaphores \n");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int supprimerFile(int msqid) {
    if(msgctl(msqid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Erreur lors de la suppression de la file Controleur \n");
        exit(EXIT_FAILURE);
    }

    printf("File supprimée.\n");
    return EXIT_SUCCESS;
}

int supprimerMemoire(int shmid) {
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Erreur lors de la suppression du segment de mémoire partagée Controleur \n");
        exit(EXIT_FAILURE);
    }

    printf("Memoire supprimée.\n");
    return EXIT_SUCCESS;
}

int supprimerSemaphores(int semid) {
    if(semctl(semid, 0, IPC_RMID) == -1) {
        fprintf(stderr, "Erreur lors de la suppresion du tableau de sémaphores Controleur \n");
        exit(EXIT_FAILURE);
    }

    printf("Tableau de sémaphores supprimé.\n");
    return EXIT_SUCCESS;
}

int ouvrir_fichier(char* nomfich){
    return(open(nomfich,O_RDWR | O_CREAT,00777));
}

int lire_fichier(int fic,unsigned char chaine [][NB_C],char * nom){
    int ret,i;
    size_t taille;
    char name [20];
    if((ret=read(fic,&taille,sizeof(size_t)))!=sizeof(size_t)){
        ncurses_stopper();
        fprintf(stderr, "Error read size: ");
        return ret;
    }
    if((ret=read(fic,name,sizeof(char)*(taille)))!=sizeof(char)*taille){
        ncurses_stopper();
        fprintf(stderr, "Error read size: ");
        return ret;
    }
    for(i=0;i<NB_L;i++){
        if((ret=read(fic,chaine[i],sizeof(unsigned char)*(NB_C)))!=sizeof(unsigned char)*(NB_C)){
        ncurses_stopper();
            fprintf(stderr, "Error read size: ");
            return ret;
        }
    }
    return ret;

}

int fermer_fichier(int fic){
    return(close(fic) == -1);
}

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

void afficheZone(unsigned char mat[][NB_C], WINDOW* win){
    int i,j,val;
    
    for(i=0;i<NB_L;i++) {
        for(j=0;j<NB_C;j++) {
            val = mat[i][j];
            
            switch (val) {
                case VIDE:
                    wattron(win, COLOR_PAIR(1));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(1));
                    break;
                case ROUTE:
                    wattron(win, COLOR_PAIR(2));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(2));
                    break;
                default:
                    wattron(win, COLOR_PAIR(3));
                    afficheMsgFen(win," ");
                    wattroff(win, COLOR_PAIR(3));
                    break;
            }
        }
    }
    wrefresh(win);
}

WINDOW* creerFenetre(int hauteur, int largeur, int posy, int posx) {
    WINDOW* fenetre;
    fenetre = newwin(hauteur, largeur, posy, posx);
    box(fenetre,0,0);
    return fenetre;
}

WINDOW* creerSousFenetre(int hauteur, int largeur, int posy, int posx, bool scroll, WINDOW* fenetre) {
    WINDOW* sous_fenetre;
    sous_fenetre = subwin(fenetre, hauteur,largeur, posy, posx);
    if (scroll == TRUE) scrollok(sous_fenetre, TRUE);
    return sous_fenetre;
}