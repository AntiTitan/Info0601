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
#define CO_UDP_CS    0
/* connexion TCP CS*/
#define CO_TCP_CS    1
/* info TCP SC*/
#define INFO_TCP_SC  2
/* création poisson SC*/
#define C_POISS      3
/* fin poisson SC*/
#define F_POISS      4
/* mouvement poisson SC*/
#define M_POISS      5
/* pose de ligne CS*/
#define P_LIGNE      6
/* autorisation ou non de la pose de ligne */
#define OKO_LIGNE    7
/* relevé de ligne CS */
#define R_LIGNE      8
/* resultat de la pêche SC */
#define PRISE        9
/* passage en mode furtif CS */
#define FURTIF_ON    10
/* fin mode furtif SC */
#define FURTIF_OFF   11
/* partie gagnée CS */
#define WIN_GAME     12
/* fin du jeu SC */
#define ENDGAME      13
/* données du jeu SC */
#define GAME         14
/*pose d'un objet par un joueur */
#define PIEGE        15

/*struct*/

/**** une seule struct pour tous les messages ****/

typedef struct message{
    struct sockaddr_in adresse;
    int position [2];/* position de la ligne ou des poissons ou dimensions de l'etang*/
    int idPoisson;
    int typePoisson;
    int direction;

    int typeMessage;
    int nbPoissons;
    int hauteur;
    int largeur;

    int idPartie;
    int idJoueur;
    
    int typeObjet;
}message_t;

typedef struct couple{
    int vide;
    struct sockaddr_in adr;
} reponseUDP_t;







#endif