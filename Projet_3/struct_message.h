#ifndef STRUCT_MESSAGE_H
#define STRUCT_MESSAGE_H

/*include*/
#include "struct_objets.h"
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/socket.h> /* Pour socket */
#include <arpa/inet.h>  /* Pour IPPROTO_TCP */
#include <stdio.h>      /* Pour perror */
#include <unistd.h>     /* Pour close */
#include <string.h>     /* Pour memset */
/*define*/

/* CS -> du client au serveur : reçu uniquement par le serveur*/
/* SC -> du serveur au client : reçu uniquement par le client*/

/* connexion UDP CS*/
#define CO_UDP_CS 0
/* connexion TCP CS*/
#define CO_TCP_CS 1
/* info TCP SC*/
#define INFO_TCP_SC  2
/* création poisson SC*/
#define C_POIS 3
/* fin poisson SC*/
#define F_POIS 4
/* mouvement poisson SC*/
#define M_POIS 5
/* pose de ligne CS*/
#define P_LIGNE 6
/* relevé de ligne CS */
#define R_LIGNE 7
/* resultat de la pêche SC */
#define PRISE 8
/* passage en mode furtif CS */
#define FURTIF_ON 9
/* fin mode furtif SC */
#define FURTIF_OFF 10
/* partie gagnée CS */
#define WIN_GAME 11
/* fin du jeu SC */
#define ENDGAME 12
/* données du jeu SC */
#define GAME 13

/*struct*/

/**** soit une seule struct pour tous les messages******/

typedef struct message{
    int typeMessage;
    struct sockaddr_in adresse;
    int position [2];/* position de la ligne ou des poissons*/
    int idJoueur;
    objet_t objet;
}message_t;







#endif