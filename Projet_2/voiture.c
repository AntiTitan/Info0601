#include "f_voiture.h"

int stopVoiture=0;
void handler_Voiture(int signal){
    if(signal == SIGINT){
        stopVoiture =1;
    }
}

int main(int argc, char * argv []){
    struct sigaction action;
    int CLE_F,CLE_M,CLE_S,rapidite,numVoiture;
    int msqid,semid, shmid;
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
        fprintf(stderr, "Nombre d'arguments incorrect:  ./voiture CLE\n");
        exit(EXIT_FAILURE);
    }
    CLE_F = atoi(argv[1]);
    rapidite = atoi(argv[2]);
    
/*récup file message -> vérifie si elle peut se connecter ou non*/
    /* Récupération de la file */
    msqid = recupererFile(CLE_F);
/*Si oui,   recup autres outils IPC (seg mem et tab sema)
            choisi une place où se mettre sur la carte
            met à jour la carte et sa position
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
    shmid = recupererMemoire;
    
    /* Récupération du tableau de sémaphores */
    semid = recupererSemaphores;

    /*cherche position libre dans liste voiture -> devient la voiture i*/
    /*Cherche une position où se placer avec mise à jour carte*/


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
        /*deplacement*/

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