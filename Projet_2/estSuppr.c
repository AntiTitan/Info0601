#include "fonctions.h"
int stopVoiture=0;
void handler_Voiture(int signal){
    if(signal == SIGINT){
        stopVoiture =1;
    }
}

int main(int argc, char * argv []){
    struct sigaction action;
    
    /* Positionnement du gestionnaire pour SIGINT */
    action.sa_handler = handler_Voiture;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    printf("pid voiture : %d\n",getpid());
    while(!stopVoiture){
        if(sigaction(SIGINT, &action, NULL) == -1) {
            perror("Erreur lors du positionnement ");
            exit(EXIT_FAILURE);
        }
    }
    /*Verification des arguments
    clé file messages
    rapidité message -> servira au timeout de ncurses
    */

    /*récup file message -> vérifie si elle eput se connecter ou non*/
    /*Si oui,   recup autres outils IPC (seg mem et tab sema)
                choisi une place où se mettre sur la carte
                met à jour la carte et sa position
    */

    /*se déplace régulièrement 
        -> met à jour sa position sur la carte (choisi sa direction ou garde sa précédente s'il n'y a pas d'obstacle)
        -> envoie message au controlleur pour indiquer un changement
    */

    /*arret sur reception SIGINT*/
    return EXIT_SUCCESS;

}