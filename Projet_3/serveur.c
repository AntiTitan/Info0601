/*include*/
#include <stdlib.h>      /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>       /* Pour printf, fprintf, perror */
#include <sys/socket.h>  /* Pour socket, bind */
#include <arpa/inet.h>   /* Pour sockaddr_in */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */

#include "struct_message.h"

int main (int argc, char * argv []){

/*déclarations*/

    int hauteur, largeur,numPort=1;
    socklen_t taille;
    int sockfdUDP, fdTCP;
    struct sockaddr_in adresseServeurUDP,adresseServeurTCP;
    struct sockaddr_in adresseClientUDP/*,adresseClientTCP*/;
    message_t reqUDP,repUDP;

/*vérification des arguments
    adresse IP
    numéro de port UDP sur lequel il va attendre les clients
    dimension de l'étang (largeur + hauteur)
*/

    if(argc != 5) {
        fprintf(stderr, "Usage : %s adresseIP port dimensions\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " adresseIP : adresse IP du serveur du serveur\n");
        fprintf(stderr, " port  : port d'écoute du serveur\n");
        fprintf(stderr, " dimensions : largeur et hauteur de la zone de jeu\n");
        exit(EXIT_FAILURE);
    }
    largeur = atoi(argv[3]);
    hauteur = atoi(argv[4]);
    largeur ++;hauteur++;

/*attente des clients en UDP*/

        /* Création de la socket UDP */

    if((sockfdUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

        /* Création de l'adresse du serveur UDP */
    memset(&adresseServeurUDP, 0, sizeof(struct sockaddr_in));
    adresseServeurUDP.sin_family = AF_INET;
    adresseServeurUDP.sin_port = htons(atoi(argv[2]));
    adresseServeurUDP.sin_addr.s_addr = htonl(INADDR_ANY);

        /* Nommage de la socket UDP */
    if(bind(sockfdUDP, (struct sockaddr*)&adresseServeurUDP, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors du nommage de la socket ");
        exit(EXIT_FAILURE);
    }

        /* Création de la socket TCP */
    if((fdTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }
        /* Création de l'adresse du serveur */
    memset(&adresseServeurTCP, 0, sizeof(struct sockaddr_in));
    adresseServeurTCP.sin_family = AF_INET;
    adresseServeurTCP.sin_addr.s_addr = htonl(atoi(argv[1]));
    adresseServeurTCP.sin_port = htons(atoi(argv[2])+numPort);
    numPort ++;

        /* Attente des clients */
    while(1){
        /* Attente de la requête du client UDP */
        printf("Serveur en attente du message du client.\n");
        taille=sizeof(struct sockaddr_in);
        if(recvfrom(sockfdUDP, &reqUDP, sizeof(message_t), 0,(struct sockaddr*)&adresseClientUDP, &taille) == -1) {
            perror("Erreur lors de la réception du message ");
            exit(EXIT_FAILURE);
        }
        printf("Serveur : requete UDP reçue.\n");

        if(reqUDP.typeMessage != CO_UDP_CS){
            printf("Problème dans la réception\n Pas le bon type de message\n");
            printf("Requete igonrée\n");
        }
        else{
            /* Envoi du message UDP */
            repUDP.typeMessage = INFO_TCP_SC;
            repUDP.adresse = adresseServeurTCP;
            if(sendto(sockfdUDP, &repUDP, sizeof(message_t), 0, (struct sockaddr*)&adresseClientUDP, sizeof(struct sockaddr_in)) == -1) {
                perror("Erreur lors de l'envoi du message ");
                exit(EXIT_FAILURE);
            }
            printf("Serveur : message envoyé.\n");    
        }
        
    }
    
/*envoi aux client des informations TCP*/

/*création de la grille et début partie*/

/*partie*/
    /*pose de ligne ->chrono*/
    /*retour de la ligne*/

    /*mode furtif*/

    /*déplacement poisson ->avertir les clients*/

/*fin quand réception gagné ou déconnexion*/  
    /* Fermeture de la socket */
    if(close(sockfdUDP) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}