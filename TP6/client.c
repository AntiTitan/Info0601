/*
Reception: seulement le type TYPE_TRIEUR
Envoie:
    TYPE_ENREGISTRER : une var de type enregistre_t
    TYPE_CLIENT : une var msgClient_t
On laisse tourner les clients, ils envoie leur pid pour s'identifier
et receveront comme id leur ordre d'enregistrement (pour le trieur)
On laisse le programme en attente d'un message à envoyer
    soit un id et un message
    soit la commande check, pour vérifier si le client a reçu un ou des messages
Les clients se terminent soient seuls -> ils envoient un message au trieur
ou quand le trieur se termine
*/

#define MAX_CLIENT 10
#define _XOPEN_SOURCE

#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h> 
#include "struct.h"

int main(int argc, char * argv []){
    int msqid,enregistre=0,enCours=1,pidd,dest;
    enregistre_t newRegister;
    msgClient_t newMsg;
    pid_t pid;
    char ordre [CHAINE_MAX], message [CHAINE_MAX];

    pid=getpid();
    pidd=(int)pid;
    /* Récupération de la file */
    if((msqid = msgget((key_t)CLE, 0)) == -1) {
        perror("Erreur lors de la récupération de la file ");
        exit(EXIT_FAILURE);
    }
    while(enCours){
        
        if(!enregistre){
            /*Enregistrement du client sur le trieur*/
            newRegister.type=TYPE_ENREGISTRE ;
            newRegister.id=pidd;
            if(msgsnd(msqid, &newRegister, sizeof(enregistre_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi de la requete ");
                exit(EXIT_FAILURE);
            }
            printf("Client %d : requete d'enregistrement envoyée.\n",pidd);

            printf("Serveur : en attente d'une requête...\n");
            if(msgrcv(msqid, &newRegister, sizeof(enregistre_t) - sizeof(long), id, 0) == -1) {
                perror("Erreur lors de la réception d'une requête ");
                exit(EXIT_FAILURE);
            }
            if(newRegister.id==-1){
                printf("Enregistrement impossible");
                return EXIT_FAILURE;
            }
            printf("Enregistrement accepté, vous êtes le client %d\n",newRegister.id);
            enregistre=1;
        }

        /*Attente d'un message à envoyer*/
        printf("Donnez une commande à executer :");
        if(scanf("%20s",ordre)!=1){
            printf("Problème avec saisie de la chaine\n");
            return EXIT_FAILURE;
        }
        /*Verification de la saisie
            quit: arrêt du client
            check: verification des messages
            msg: envoi d'un message à un client
        */
        if(ordre=="msg"){
            /*Envoi message*/
            printf("Destinataire :");
            if(scanf("%d",dest)!=1){
                printf("Problème avec saisie du destinataire\n");
                return EXIT_FAILURE;
            }
            printf("Message : ");
            if(scanf("%20s",message)!=1){
                printf("Problème avec saisie du destinataire\n");
                return EXIT_FAILURE;
            }
            newMsg.type=TYPE_CLIENT;
            newMsg.id=newRegister.id;
            newMsg.dest=dest;
            strcpy(newMsg.chaine,message);
            if(msgsnd(msqid, &newMsg, sizeof(msgClient_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi du message ");
                exit(EXIT_FAILURE);
            }
            printf("Client %d : message envoyé.\n",newRegister.id);
        }
        
        if(ordre=="check"){
            /*Checking des messages reçus*/
        }
        /*Suppr du client du trieur*/
    }
   

    return EXIT_SUCCESS;
}