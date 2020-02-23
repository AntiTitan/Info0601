#include "f_voitures.h"

int main(int argc, char * argv []){

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

}