#include "f_controleur.h"

int main (int argc, char * argv []){

/*Verification des arguments
    nom fichier carte
    nb max de voitures
    clé file messages
    clé seg memoire partagée
    cle tableau sémaphores
*/

/*Création outils IPC*/

/*Init outils IPC*/

        /*Premier affichage simulation (on gère ça à la fin)*/
/*Mise en attente messages sur file*/

/*Mise à jour des infos suite à la reception d'un message*/
        /*Mise à jour affichage (on gère ça à la fin) */
/*Reception message : gérér
    - cas où une voiture change sa position (TYPE 3)
        -> modif info sur seg mem partagé
    - cas où une voiture souhaite s'enregistrer (TYPE 1)
        -> renvoi message TYPE 2 avec clé seg mem partagée et tab séma*/


/*Arret sur SIGINT (ou utilisateur) -> arret de toutes les voitures avec SIGINT */
/*Envoi SIGINT aux programmes voiture*/

/*suppression outils IPC */

/*arrêt prgm*/

}