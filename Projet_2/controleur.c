#include "f_controleur.h"

int stopControleur=0;
int arret[3];

void handler_Controleur(int signal){
    pid_t pid;
    if(signal == SIGINT){
        printf("recu controller\nnum pid ? ");
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
    int i,fic;
    unsigned short val[NBSEM];/*tableau d'initialisation des sem*/
    /*1 semaphore pour la carte entière ou un semaphore pour une zone 5x5 ?*/
    pid_t voitures [MAX_VOITURE];
    info_t *map;
    struct sigaction action;

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

/*Allocation de la structure partagee*/

    if ((map = malloc(sizeof(info_t)))==NULL) {
        fprintf(stderr, "erreur malloc");
        exit(EXIT_FAILURE);
    }
    size = sizeof(map);

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
        fprintf(stderr, "Erreur lors de l'initialisation des sémaphores ");
        exit(EXIT_FAILURE);
    }
/*Remplissage seg memoire*/
    /*Extraction info du fichier*/
    if((fic = ouvrir_fichier(argv[1]))==-1){
        fprintf(stderr, "Erreur lors de l'ouverture du fichier carte ");
        exit(EXIT_FAILURE);
    }
    if(lire_fichier(fic, map->carte->grille, argv[1])==-1){
        fprintf(stderr, "Erreur lors de la lecture du fichier carte ");
        exit(EXIT_FAILURE);
    }
    /*Attachement du segment memoire partagée*/

    /*Premier affichage simulation (on gère ça à la fin)*/

/*Arret sur SIGINT (ou utilisateur) -> arret de toutes les voitures avec SIGINT */
/*Envoi SIGINT aux programmes voiture*/
    while(!stopControleur){
        if(sigaction(SIGINT, &action, NULL) == -1) {
            perror("Erreur lors du positionnement ");
            exit(EXIT_FAILURE);
        }
        /*Mise en attente messages sur file*/

        /*Mise à jour des infos suite à la reception d'un message*/
            /*Mise à jour affichage (on gère ça à la fin) */
        /*Reception message : gérér
            - cas où une voiture change sa position (TYPE 3)
                -> modif info sur seg mem partagé
            - cas où une voiture souhaite s'enregistrer (TYPE 1)
                -> renvoi message TYPE 2 avec clé seg mem partagée et tab séma*/
    }
/*suppression outils IPC */

    supprimerFile(msqid);
    supprimerMemoire(shmid);
    supprimerSemaphores(semid);
    free(map);
/*arrêt prgm*/
    return EXIT_SUCCESS;
}