#define _XOPEN_SOURCE
#include "fcontroleur.h"

int stopControleur=0;
int arret[3];

void handler_Controleur(int signal){
    pid_t pid;
    if(signal == SIGINT){
        printw("recu controller\nnum pid ? ");
        if(scanf("%d",&pid)==-1){
            exit(EXIT_FAILURE);
        }
        kill(pid,SIGINT);
        /*suppression outils IPC */
        supprimerFile(arret[0]);
        supprimerMemoire(arret[1]);
        supprimerSemaphores(arret[2]);
        stopControleur =1;
    }
}

int main (int argc, char * argv []){
    int nbV, CLE_F, CLE_M, CLE_S;
    int msqid, shmid, semid, size;
    int i,fic,j,trouve;
    unsigned short val[NBSEM];/*tableau d'initialisation des sem*/
    /*1 semaphore pour la carte entière ou un semaphore pour une zone 5x5 ?*/
    pid_t voitures [MAX_VOITURE];
    info_t *map;
    struct sigaction action;
    r_config_t rconfig;
    e_config_t econfig;
    modif_carte_t modif;
    WINDOW * fenetre, *sous_fen;
/*Verification des arguments
    nom fichier carte
    nb max de voitures
    clé file messages
    clé seg memoire partagée
    cle tableau sémaphores
*/

    if (argc != 6) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./controleur ficCartte nbMaxVoiture CLE_FIL CLE_MEM, CLE_SEM\n");
        exit(EXIT_FAILURE);
    }
    nbV = atoi(argv[2]);
    CLE_F = atoi(argv[3]);
    CLE_M = atoi(argv[4]);
    CLE_S = atoi(argv[5]);
    
    /* Positionnement du gestionnaire pour SIGINT */
    action.sa_handler = handler_Controleur;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

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

/*Allocation de la structure partagee*/

    if ((map = malloc(sizeof(info_t)))==NULL) {
        ncurses_stopper();
        fprintf(stderr, "erreur malloc");
        exit(EXIT_FAILURE);
    }
    size = sizeof(map);
    for(j=0;j<MAX_VOITURE;j++){
        voitures[j]=0;
        map->position[j][0]=-1;
        map->position[j][1]=-1;
    }

/*Création outils IPC*/

    msqid = creerFile(CLE_F);
    shmid = creerMemoire(CLE_M, size);
    semid = creerSemaphores(CLE_S);
    arret[0]=msqid;
    arret[1]=shmid;
    arret[2]=semid;

/*Init outils IPC*/

    /* Initialisation du tab d'initialisation des sem*/
    for (i=0; i<NBSEM; i++) {
        val[i] = 1;
    }

    /* Initialisation des sémaphores */
    if(semctl(semid, 0, SETALL, val) == -1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'initialisation des sémaphores ");
        exit(EXIT_FAILURE);
    }
/*Remplissage seg memoire*/
    /*Extraction info du fichier*/
    if((fic = ouvrir_fichier(argv[1]))==-1){
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'ouverture du fichier carte ");
        exit(EXIT_FAILURE);
    }
    if(lire_fichier(fic, map->carte.grille, argv[1])==-1){
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de la lecture du fichier carte ");
        exit(EXIT_FAILURE);
    }

    /* Attachement de la map au segment de mémoire partagée */
    if((map = shmat(shmid, NULL, 0)) == (void*)-1) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
    printw("attachement au segment de memoire.");
    
    /*Premier affichage simulation (on gère ça à la fin)*/
    /* Création de la fenêtre d'affichage*/
    fenetre = creerFenetre(HAUTEUR, LARGEUR, POSY, POSX);
    sous_fen = creerSousFenetre(HAUTEUR - 2, LARGEUR - 2, POSY + 1, POSX + 1, FALSE, fenetre);

    /* Definition de la palette */
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_BLUE, COLOR_WHITE);

    /* Colore le fond de la fenêtre */
    wbkgd(fenetre, COLOR_PAIR(4));
    wrefresh(fenetre);
    wbkgd(sous_fen, COLOR_PAIR(4));
    wrefresh(sous_fen);

/*Arret sur SIGINT (ou utilisateur) -> arret de toutes les voitures avec SIGINT */
/*Envoi SIGINT aux programmes voiture*/
    while(!stopControleur){
        if(sigaction(SIGINT, &action, NULL) == -1) {
            ncurses_stopper();
            fprintf(stderr, "Erreur lors du positionnement ");
            exit(EXIT_FAILURE);
        }
        /*Mise en attente messages sur file*/
        /* Attente d'une requête d'enregistrement de voiture */
        /*printw("Serveur : en attente d'une requête...");*/
        if(msgrcv(msqid, &rconfig, sizeof(r_config_t) - sizeof(long), TYPE_RECUP_CONFIG, IPC_NOWAIT) == -1) {
            if(errno!=ENOMSG){
                ncurses_stopper();
                fprintf(stderr, "Erreur lors de la réception d'une requête ");
                exit(EXIT_FAILURE);
            }
            /*Pas de messages TYPE_RECUP_CONFIG*/ 
        }
        else{
            /*Enregistrement de la voiture*/
            trouve =0;
            j=0;
            while(j<MAX_VOITURE && !trouve){
                if(voitures[j]==0){
                    j++;
                }
                else{
                    trouve=1;
                }
            }
            if(trouve){
                voitures[j]=rconfig.pid;
            }
            
            /*Envoi clé à la voiture*/
            econfig.type=TYPE_ENVOI_CONFIG;
            econfig.cle_mem=CLE_M;
            econfig.cle_sema=CLE_S;

            if(msgsnd(msqid, &econfig, sizeof(e_config_t) - sizeof(long), 0) == -1) {
                ncurses_stopper();
                fprintf(stderr, "Erreur lors de l'envoi de la requête ");
                exit(EXIT_FAILURE);
            }
        }

        /* Attente d'une requête de modif de positionnement */
        if(msgrcv(msqid, &modif, sizeof(modif_carte_t) - sizeof(long), TYPE_MODIF_CARTE, IPC_NOWAIT) == -1) {
            if(errno!=ENOMSG){
                ncurses_stopper();
                fprintf(stderr, "Erreur lors de la réception d'une requête ");
                exit(EXIT_FAILURE);
            }
            /*Pas de messages TYPE_MODIF_CARTE*/ 
        }
        /*Affichage de la simulation*/
        else{
            /*P(Semaphore info)*/
            Peux(0,CLE_S);
            /*affichage avec ncurses*/
            afficheZone(map->carte.grille, sous_fen);
            /*V(Semaphore info)*/
            Vas(0,CLE_S);
        }
    }
    ncurses_stopper();
    
/*suppression outils IPC */
    supprimerFile(msqid);
    supprimerMemoire(shmid);
    supprimerSemaphores(semid);
    free(map);
    
/*arrêt prgm*/
    return EXIT_SUCCESS;
}