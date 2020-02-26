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
    int msqid,semid,segid;
    r_config_t requete_r;
    e_config_t requete_e;
    modif_carte_t modification;
    
    /* Positionnement du gestionnaire pour SIGINT */
    action.sa_handler = handler_Voiture;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    
    requete_r.type = TYPE_RECUP_CONFIG;
    modification.type =TYPE_MODIF_CARTE;
    /*Verification des arguments
    clé file messages
    rapidité message -> servira au timeout de ncurses
    */
    if (argc != 3) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./controleur ficCartte nbMaxVoiture CLE_FIL CLE_MEM, CLE_SEM\n");
        exit(EXIT_FAILURE);
    }
    CLE_F = atoi(argv[1]);
    rapidite = atoi(argv[2]);
    
/*récup file message -> vérifie si elle eput se connecter ou non*/
    /* Récupération de la file */
    if((msqid = msgget((key_t)CLE_F, 0)) == -1) {
        perror("Erreur lors de la récupération de la file ");
        exit(EXIT_FAILURE);
    }
/*Si oui,   recup autres outils IPC (seg mem et tab sema)
            choisi une place où se mettre sur la carte
            met à jour la carte et sa position
*/
    /* Envoi d'une requête */
    
    requete_r.vpid = getpid();

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
    CLE_M=requete_e.cle_mem;
    CLE_S=requete_e.cle_sema;

    /*Recupération segment mémoire*/
    if((shmid = shmget((key_t)CLE_M, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du segment de mémoire ");
        exit(EXIT_FAILURE);
    }
    
    /* Récupération du tableau de sémaphores */
    if((semid = semget((key_t)CLE_S, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphores ");
        exit(EXIT_FAILURE);
    }
    /*cherche position libre dans liste voiture -> nevient la voiture i*/
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