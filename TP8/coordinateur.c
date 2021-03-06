#include "TP8.h"


#define LARGEUR  32     /* Largeur de la fenêtre */
#define HAUTEUR  17     /* Hauteur de la fenêtre */
#define POSX     0      /* Position horizontale de la fenêtre */
#define POSY     3      /* Position verticale de la fenêtre */
#define NBSEM    NB_C*NB_L



int main(int argc, char* argv[]) {
    int shmid, semid, i, CLE_S, CLE_M,k;
    unsigned short val[NBSEM];/*tableau d'initialisation des sem*/
    grille_t* grille;
    WINDOW* fenetre, * sous_fenetre, *info;

    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();

    if (argc != 3) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./coordinateur CLE_MEM, CLE_SEM\n");
        ncurses_stopper();
        exit(EXIT_FAILURE);
    }

    if ((grille = malloc(sizeof(grille_t)))==NULL) {
        fprintf(stderr, "erreur malloc");
        ncurses_stopper();
        exit(EXIT_FAILURE);
    }
    CLE_M = atoi(argv[1]);
    CLE_S = atoi(argv[2]);

    /* Création du tableau de sémaphores */
    if((semid = semget((key_t)CLE_S, NBSEM, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        ncurses_stopper();
        if(errno == EEXIST)
            fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE_S);
        else
            perror("Erreur lors de la création du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }

    /* Initialisation du tab d'initialisation des sem*/
    for (i=0; i<NBSEM; i++) {
        val[i] = 0;
    }

    /* Initialisation des sémaphores */
    if(semctl(semid, 0, SETALL, val) == -1) {
        perror("Erreur lors de l'initialisation des sémaphores ");
        ncurses_stopper();
        exit(EXIT_FAILURE);
    }

    /* Initialisation de la grille */
    initZone(grille->grille);

    

    /* Definition de la palette */
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_YELLOW);
    init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(6, COLOR_WHITE, COLOR_CYAN);
    init_pair(7, COLOR_BLUE, COLOR_WHITE);

    /* Création d'un segment d'un grille de 15x30 message */
    if((shmid = shmget((key_t)CLE_M, sizeof(grille), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        ncurses_stopper();
        if(errno == EEXIST) {
            fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE_M);
        }
        else
            fprintf(stderr, "Erreur lors de la création du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    /*printf("Serveur : segment mémoire partagée créé.\n");*/

    /* Attachement du segment de mémoire partagée */
    if((grille = shmat(shmid, NULL, 0)) == (void*)-1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }


    /* Vérification des dimensions du terminal */
    if((COLS < POSX + LARGEUR) || (LINES < POSY + HAUTEUR)) {
        ncurses_stopper();
        fprintf(stderr, 
            "Les dimensions du terminal sont insufisantes : l=%d,h=%d au lieu de l=%d,h=%d\n", 
            COLS, LINES, POSX + LARGEUR, POSY + HAUTEUR);
        exit(EXIT_FAILURE);
    }  

    /* Création de la fenêtre d'affichage*/
    fenetre = creerFenetre(HAUTEUR, LARGEUR, POSY, POSX);
    info = creerFenetre(HAUTEUR, LARGEUR, POSY, POSX + LARGEUR);
    sous_fenetre = creerSousFenetre(HAUTEUR - 2,LARGEUR - 2, POSY + 1, POSX + 1, FALSE, fenetre);

    /* Colore le fond de la fenêtre */
    wbkgd(fenetre, COLOR_PAIR(7));
    wrefresh(fenetre);
    wbkgd(info, COLOR_PAIR(6));
    wrefresh(info);
    wbkgd(sous_fenetre, COLOR_PAIR(7));
    wrefresh(sous_fenetre);

    afficheMsgFen(info,"IIII");

    printw("Cliquez dans la fenetre ; pressez F2 pour quitter...");  
    timeout(500);
    while((k = getch()) != KEY_F(2)) {
        /* Affichage de la grille du segment de mémoire partagée */
        printw("affichZone.");
        afficheZone(grille->grille, sous_fenetre, CLE_S);
        
    }

    /* Suppression des fenêtres et sous-fenêtres*/
    delwin(fenetre);
    delwin(info);
    delwin(sous_fenetre);

    /* Arrêt de ncurses */
    ncurses_stopper();

    /* Détachement du segment de mémoire partagée */
    if(shmdt(grille) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors du détachement ");
        exit(EXIT_FAILURE);
    }

    /* Suppression du segment de mémoire partagée */
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Erreur lors de la suppression du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }


    /* Suppression du tableau de sémaphores */
    if(semctl(semid, 0, IPC_RMID) == -1) {
        fprintf(stderr, "Erreur lors de la suppresion du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }

    free(grille);

    return EXIT_SUCCESS;
}