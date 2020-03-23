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

    int sockfdUDP, sockfdTCP;
    struct sockaddr_in adresseServeurUDP , adresseServeurTCP;
    message_t reqUDP,repUDP;
    message_t reqTCP,repTCP;

/*vérification des arguments
    adresse IP
    numéro de port UDP du serveur
*/

   if(argc != 3) {
        fprintf(stderr, "Usage : %s adresseIP port \n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " adresseIP : adresse IP du serveur du serveur\n");
        fprintf(stderr, " port  : port d'écoute du serveur\n");
        exit(EXIT_FAILURE);
    }
    reqUDP.typeMessage = CO_UDP_CS;

/*connexion au serveur en UDP*/

        /* Création de la socket UDP */
    if((sockfdUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }
        /* Création de l'adresse du serveur */
    memset(&adresseServeurUDP, 0, sizeof(struct sockaddr_in));
    adresseServeurUDP.sin_family = AF_INET;
    adresseServeurUDP.sin_port = htons(atoi(argv[2]));
    if(inet_pton(AF_INET, argv[1], &adresseServeurUDP.sin_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }
        /* Envoi du message */
    if(sendto(sockfdUDP, &reqUDP, sizeof(message_t), 0, (struct sockaddr*)&adresseServeurUDP, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
    }
    printf("Client : message envoyé.\n");

/*reception des informations TCP*/

        /* Attente de la reponse du serveur */
    if(recvfrom(sockfdUDP, &repUDP, sizeof(message_t), 0, NULL, NULL) == -1) {
      perror("Erreur lors de la réception du message ");
      exit(EXIT_FAILURE);
    }

    if(repUDP.typeMessage != INFO_TCP_SC){
        printf("Mauvais type de message\nRequete ignorée\n");
    }
    else{
        printf("Info TCP reçues\n");
    }
    adresseServeurTCP = repUDP.adresse;

        /* Création de la socket TCP */
    if((sockfdTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

    if(inet_pton(AF_INET, argv[1], &adresseServeurTCP.sin_addr.s_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }
    
/*connexion au serveur en TCP*/

        /* Connexion au serveur */
    if(connect(sockfdTCP, (struct sockaddr*)&adresseServeurTCP, sizeof(adresseServeurTCP)) == -1) {
        perror("Erreur lors de la connexion ");
        exit(EXIT_FAILURE);
    }
    reqTCP.typeMessage=CO_TCP_CS;

    if(write(sockfdTCP, &reqTCP , sizeof(message_t)) == -1) {
      perror("Erreur lors de l'envoi du message ");
      exit(EXIT_FAILURE);
    }
    printf("Client : message TCP envoyé.\n");

    if(read(sockfdTCP, &repTCP, sizeof(message_t)) == -1) {
      perror("Erreur lors de la lecture de la taille du message ");
      exit(EXIT_FAILURE);
    }
    printf("Client : message TCP recu.\n");
    if(repTCP.typeMessage==GAME){
        printf("Bon type !\n");
    }
    /*reception des données de la partie -> joueur et grille*/

    /*création locale de la grille*/

    /*début de partie -> tant que pas fin du jeu (ou deconnexion -> à gérer)*/

        /*attente de messages du serveur -> thread affichage*/
        
        /*pose de la ligne -> envoi message au serveur*/

        /*remonte la ligne -> debut chrono du tour et envoi message*/
        /*message du serveur -> attend la prise et réagit*/

        /*pose d'objet -> envoi message*/

        /*message du serveur -> victime de dynamite*/

    /*message du serveur -> fin de partie*/

    /* Fermeture de la socket */
    if(close(sockfdUDP) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}