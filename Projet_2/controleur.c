#include "f_controleur.h"

int main (int argc, char * argv []){
    int nbV, CLE_F, CLE_M, CLE_S;
    int msqid, shmid, semid, size;
    unsigned short val[NBSEM];/*tableau d'initialisation des sem*/
    grille_t carte;

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
    size = sizeof(carte);

/*Allocation de la structure partagee*/

    if ((carte = malloc(sizeof(grille_t)))==NULL) {
        fprintf(stderr, "erreur malloc");
        exit(EXIT_FAILURE);
    }


/*Création outils IPC*/

    msqid = creerFile(CLE_F);
    shmid = creerMemoire(CLE_M, size);
    semid = creerSemaphores(CLE_S);

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

    supprimerFile(msqid);
    supprimerMemoire(shmid);
    supprimerSemaphores(semid);
    free(carte);
/*arrêt prgm*/

}