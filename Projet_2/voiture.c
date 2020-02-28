#define _XOPEN_SOURCE
#include "fvoiture.h"

#define HAUT 1
#define DROITE 2
#define BAS 3
#define GAUCHE 4

int stopVoiture=0;
void handler_Voiture(int signal){
    if(signal == SIGINT){
        stopVoiture =1;
    }
}

int main(int argc, char * argv []){
    struct sigaction action;
    int CLE_F,CLE_M,CLE_S,rapidite,numVoiture;
    int msqid,semid, shmid, i, d, posx, posy, cpt;
    info_t *map;
    r_config_t requete_r;
    e_config_t requete_e;
    modif_carte_t modification;
    
    /* Positionnement du gestionnaire pour SIGINT */
    action.sa_handler = handler_Voiture;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    
    requete_r.type = TYPE_RECUP_CONFIG;
    modification.type = TYPE_MODIF_CARTE;

    /*Verification des arguments
    clé file messages
    rapidité message -> servira au timeout de ncurses
    */
    if (argc != 3) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./voiture CLE_FILE\n");
        exit(EXIT_FAILURE);
    }
    CLE_F = atoi(argv[1]);
    rapidite = atoi(argv[2]);
    
/*récup file message -> vérifie si elle peut se connecter ou non*/
    /* Récupération de la file */
    msqid = recupererFile(CLE_F);
/*Si oui,   recup autres outils IPC (seg mem et tab sema)
  choisi une place où se mettre sur la carte met à jour 
  la carte et sa position
*/
    /* Envoi d'une requête */
    
    requete_r.pid = getpid();

    if(msgsnd(msqid, &requete_r, sizeof(r_config_t) - sizeof(long), 0) == -1) {
        perror("Erreur lors de l'envoi de la requête ");
        exit(EXIT_FAILURE);
    }
    printf("Voiture %d : envoi d'une requête.\n",getpid());

    /* Réception de la réponse */
    printf("Client : attente de la reponse...\n");
    if(msgrcv(msqid, &requete_e, sizeof(e_config_t) - sizeof(long), TYPE_ENVOI_CONFIG, 0) == -1) {
        perror("Erreur lors de la réception de la réponse ");
        exit(EXIT_FAILURE);    
    }
    CLE_M = requete_e.cle_mem;
    CLE_S = requete_e.cle_sema;

    /*Recupération segment mémoire*/
    shmid = recupererMemoire(CLE_M);
    
    /* Récupération du tableau de sémaphores */
    semid = recupererSemaphores(CLE_S);

    printf("%d",semid);
    printf("%d",rapidite);

    /* Attachement de la map au segment de mémoire partagée */
    if((map = shmat(shmid, NULL, 0)) == (void*)-1) {
        fprintf(stderr, "Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
    printf("attachement au segment de memoire\n");

    /*cherche position libre dans liste voiture -> devient la voiture i*/
    i = 0;
    while(map->position[i][0]!=-1 && i<MAX_VOITURE) {
        i++;
    }
    numVoiture = i+1;

    /*Cherche une position où se placer avec mise à jour carte*/
    /*********************************************************A FAIRE**************************************************************/
    /*dans la grille 0 vide, 1 route, 2 pour la voiture num 1, 3 pour la voiture num 2 ... */
    /*choix aleatoire d'une case de la grille si c un obstacle la voiture peut s'y positionner sinon on retire une case au hasard*/
    
    /*init map->position[numVoiture-1][0] et map->position[numVoiture-1][1]*/

    /*mise à jour position dans seg memoire*/



    /*arret sur reception SIGINT*/
    while(!stopVoiture){
        if(sigaction(SIGINT, &action, NULL) == -1) {
            perror("Erreur lors du positionnement ");
            exit(EXIT_FAILURE);
        }
        /*se déplace régulièrement 
            -> met à jour sa position sur la carte (choisi sa direction ou garde sa précédente s'il n'y a pas d'obstacle)
            -> envoie message au controlleur pour indiquer un changement
        */
        /*P(Semaphore du seg memoire)*/

        /*deplacement*/
        posx = map->position[numVoiture-1][0];
        posy = map->position[numVoiture-1][1];
        d = alea(1, 4);
        cpt = 0;

        /*si la voiture est sur la premiere/derniere ligne/colonne (certaine(s) direction(s) impossible(s)) ou s'il n'y a pas 
          de route dans la direction choisie ou s'il y a deja une voiture, on essaye la direction suivante si apres avoir tente
          toutes les directions aucune n'est possible, on ne bouge bas
        */
        while (cpt<4) {
            switch (d) {
                case HAUT:/*hesitation entre && et || dans les if*/
                    if(posx == 0 && map->carte.grille[posx-1][posy] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        cpt = 10;
                    }
                    break;
                case DROITE:
                    if(posy == NB_C-1 && map->carte.grille[posx-1][posy] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        cpt = 10;
                    }
                    break;
                case BAS:
                    if(posx == NB_L-1 && map->carte.grille[posx-1][posy] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        cpt = 10;
                    }
                    break;
                case GAUCHE:
                    if(posy == 0 && map->carte.grille[posx-1][posy] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        cpt = 10;
                    }
                    break;
            }
        }
        if (cpt != 10) {
            /*pas de deplacement*/
            printf("impossible de se deplacer\n");
        }
        else {
            switch (d) {
                case HAUT:
                    /*la voiture se deplace sur la case du haut*/
                    break;
                case DROITE:
                    /*la voiture se deplace sur la case de droite*/
                    break;
                case BAS:
                    /*la voiture se deplace sur la case du bas*/
                    break;
                case GAUCHE:
                    /*la voiture se deplace sur la case de gauche*/
                    break;
            }
        }
        /*V(Semaphore du seg memoire)*/

        /*avertissement changement position*/
        modification.voiture=numVoiture;

        /*envoi message*/
        if(msgsnd(msqid, &modification, sizeof(modif_carte_t) - sizeof(long), 0) == -1) {
            perror("Erreur lors de l'envoi de la requête ");
            exit(EXIT_FAILURE);
        }
        printf("Voiture %d : envoi d'une requête de modif.\n",numVoiture);

    }
    return EXIT_SUCCESS;

}