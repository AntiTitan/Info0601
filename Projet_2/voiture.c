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
    int msqid,semid, shmid, i, d, posx, posy, cpt, libre;
    info_t *map;
    r_config_t requete_r;
    e_config_t requete_e;
    modif_carte_t modification;

    /*Verification des arguments
    clé file messages
    rapidité message -> servira au timeout de ncurses
    */
    if (argc != 3) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./voiture CLE_FILE rapidite\n");
        exit(EXIT_FAILURE);
    }
    CLE_F = atoi(argv[1]);
    rapidite = atoi(argv[2]);
    
    /* Positionnement du gestionnaire pour SIGINT */
    action.sa_handler = handler_Voiture;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    
    requete_r.type = TYPE_RECUP_CONFIG;
    requete_r.pid = getpid();
    modification.type = TYPE_MODIF_CARTE;
    
/*récup file message -> vérifie si elle peut se connecter ou non*/
    msqid = recupererFile(CLE_F);

/*Si oui,   recup autres outils IPC (seg mem et tab sema)
  choisi une place où se mettre sur la carte met à jour 
  la carte et sa position
*/
    /* Envoi requête pour recuperer les cles*/

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

    shmid = recupererMemoire(CLE_M);
    semid = recupererSemaphores(CLE_S);
/*test d'affichage*/
    printf("%d\n",semid);
    printf("%d\n",rapidite);
/******************/

    /* Attachement de la map au segment de mémoire partagée */
    if((map = shmat(shmid, NULL, 0)) == (void*)-1) {
        fprintf(stderr, "Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
    printf("attachement au segment de memoire\n");

    /*cherche position libre dans liste voiture -> devient la voiture i*/
    Peux(0,CLE_S);
    i = 0;
    while(map->position[i][0]!=-1 && i<MAX_VOITURE) {
        i++;
    }
    Vas(0,CLE_S);
    if(i==MAX_VOITURE){
        fprintf(stderr,"Il y a dejà trop de voitures\n");
        exit(EXIT_FAILURE);
    }
    numVoiture = i+1;
    modification.voiture = numVoiture;

    printf("position voiture libre trouvee\n");
    /*Cherche une position où se placer avec mise à jour carte*/
    /*********************************************************A FAIRE**************************************************************/
    /*dans la grille 0 vide, 1 route, 2 pour la voiture num 1, 3 pour la voiture num 2 ... */
    /*choix aleatoire d'une case de la grille si c un obstacle la voiture peut s'y positionner sinon on retire une case au hasard*/
    libre = 0;
    while(!libre) {
        posx = alea(0, NB_L-1);
        posy = alea(0, NB_C-1);
        Peux(0, CLE_S);
        if(map->carte.grille[posx][posy] == ROUTE) {
        /*init map->position[numVoiture-1][0] et map->position[numVoiture-1][1]*/
            /*mise à jour position dans seg memoire*/
            map->position[numVoiture-1][0] = posx;
            map->position[numVoiture-1][1] = posy;
            /*mise à jour de la grille*/
            map->carte.grille[posx][posy] = numVoiture;
            libre = 1;
        }
        Vas(0,CLE_S);
        
    }

    printf("emplacement libre trouve: x %d, y %d\n",posx,posy);



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
        printf("boucle while avant Peux\n");
        Peux(0,CLE_S);
        printf("boucle while apres Peux\n");
        /*deplacement*/
        posx = map->position[numVoiture-1][0];
        posy = map->position[numVoiture-1][1];
        d = alea(1, 4);
        cpt = 0;
        printf("boucle while avant tentative de deplacement %d\n",d);

        /*si la voiture est sur la premiere/derniere ligne/colonne (certaine(s) direction(s) impossible(s)) ou s'il n'y a pas 
          de route dans la direction choisie ou s'il y a deja une voiture, on essaye la direction suivante si apres avoir tente
          toutes les directions aucune n'est possible, on ne bouge bas
        */
        while (cpt<4) {
            switch (d) {
                case HAUT:/*hesitation entre && et || dans les if*/
                    if(posx == 0 || map->carte.grille[posx-1][posy] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        map->carte.grille[posx-1][posy] = numVoiture;
                        map->carte.grille[posx][posy] = ROUTE;
                        cpt=4;
                    }
                    break;
                case DROITE:
                    if(posy == NB_C-1 || map->carte.grille[posx][posy+1] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        map->carte.grille[posx][posy+1] = numVoiture;
                        map->carte.grille[posx][posy] = ROUTE;
                        cpt=4;
                    }
                    break;
                case BAS:
                    if(posx == NB_L-1 || map->carte.grille[posx+1][posy] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        map->carte.grille[posx+1][posy] = numVoiture;
                        map->carte.grille[posx][posy] = ROUTE;
                        cpt=4;
                    }
                    break;
                case GAUCHE:
                    if(posy == 0 || map->carte.grille[posx][posy-1] != ROUTE) {
                        d = (d+1)%4;
                        cpt++;
                    }
                    else {
                        map->carte.grille[posx][posy-1] = numVoiture;
                        map->carte.grille[posx][posy] = ROUTE;
                        cpt=4;
                    }
                    break;
            }
        }
        printf("boucle while apres recherche de deplacement\n");
        if (cpt == 4) {
            /*pas de deplacement*/
            printf("impossible de se deplacer\n");
        }
        else {
            printf("boucle while tentative deplacement\n");
            /*avertissement changement position*/
            /*envoi message*/
            if(msgsnd(msqid, &modification, sizeof(modif_carte_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi de la requête ");
                exit(EXIT_FAILURE);
            }
            printf("Voiture %d : envoi d'une requête de modif.\n",numVoiture);

        }
        /*V(Semaphore du seg memoire)*/
        Vas(0,CLE_S);
        sleep(2);
    }
    return EXIT_SUCCESS;

}