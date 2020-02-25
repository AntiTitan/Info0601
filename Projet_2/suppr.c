#include "fonctions.h"

int stopControleur=0;

void handler_Controleur(int signal){
    pid_t pid;
    if(signal == SIGINT){
        printf("recu controller\nnum pid ? ");
        if(scanf("%d",&pid)==-1){
            exit(EXIT_FAILURE);
        }
        kill(pid,SIGINT);
        stopControleur =1;
    }
}

int main (int argc, char * argv []){
    
/*Verification des arguments
    nom fichier carte
    nb max de voitures
    clé file messages
    clé seg memoire partagée
    cle tableau sémaphores
*/
    struct sigaction action;

    /* Positionnement du gestionnaire pour SIGINT */
    action.sa_handler = handler_Controleur;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
  
    while(!stopControleur){
        if(sigaction(SIGINT, &action, NULL) == -1) {
            perror("Erreur lors du positionnement ");
            exit(EXIT_FAILURE);
        };
    }
    return EXIT_SUCCESS;

}