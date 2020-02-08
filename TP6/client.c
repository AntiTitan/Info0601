/*
Reception: 
    le type égal à son pid: réception d'un msg
    TYPE_ENREGISTRE : vérification de la demande d'enregistrement
Envoie:
    TYPE_ENREGISTRE : une var de type enregistre_t
    TYPE_CLIENT : une var msgClient_t
On laisse tourner les clients, ils envoie leur pid pour s'identifier
et receveront comme id leur ordre d'enregistrement (pour le trieur)
On laisse le programme en attente d'un message à envoyer
    soit un id et un message
    soit la commande check, pour vérifier si le client a reçu un ou des messages
Les clients se terminent soit seuls -> ils envoient un message au trieur
ou quand le trieur se termine
*/

#define _XOPEN_SOURCE

#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h> 
#include "struct.h"

void viderBuffer(){
    int c = 0;
    while (c != '\n' && c != EOF){
        c = getchar();
    }
}

int main(int argc, char * argv []){
    int msqid,enregistre=0,enCours=1,pidd,dest, ordre=0;
    enregistre_t newRegister;
    msgClient_t newMsg;
    requete_t msgRecu;
    pid_t pid;
    char /*ordre [CHAINE_MAX],*/ message [CHAINE_MAX];

    pid=getpid();
    pidd=(int)pid;

    /* Récupération de la file */
    if((msqid = msgget((key_t)CLE, 0)) == -1) {
        perror("Erreur lors de la récupération de la file (Client)");
        exit(EXIT_FAILURE);
    }

    while(enCours){
        
        if(!enregistre){
            /*Enregistrement du client sur le trieur*/
            newRegister.type=TYPE_ENREGISTRE ;
            newRegister.id=pidd;
            if(msgsnd(msqid, &newRegister, sizeof(enregistre_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de la demande d'enregistrement ");
                exit(EXIT_FAILURE);
            }
            printf("Client %d : requete d'enregistrement envoyée.\n",pidd);

            printf("Client : en attente d'une réponse...\n");
            if(msgrcv(msqid, &newRegister, sizeof(enregistre_t) - sizeof(long), pidd, 0) == -1) {
                perror("Erreur lors de l'attente de la verification d'enregistrement ");
                exit(EXIT_FAILURE);
            }
            if(newRegister.id==-1){
                printf("Enregistrement impossible, limite de client atteinte");
                return EXIT_FAILURE;
            }
            printf("Enregistrement accepté, je suis le client %d\n",newRegister.id);
            enregistre=1;
        }

        /*Demande au client ce qu'il veut faire*/
        printf("Donnez une commande à executer (1, 2 ou 3):\n");
        printf("\t1_quit: arrêt du client\n");
        printf("\t2_check: verification des messages\n");
        printf("\t3_msg: envoi d'un message à un client\n");
        /*Demande jusqu'a ce que l'ordre soit correct'*/
        while(ordre<1 || ordre>3) {
            viderBuffer();
            if(scanf("%d",&ordre)!=1){
                printf("Problème avec saisie de l'ordre\n");
                return EXIT_FAILURE;
            }
        }

        /*Execution de l'ordre recu*/
        if(ordre==3){
            /*Envoi d'un message*/
            printf("\nLe client envoie un message\n");
            printf("Destinataire : ");
            viderBuffer();
            if(scanf("%d",&dest)!=1){
                printf("Problème avec saisie du destinataire\n");
                return EXIT_FAILURE;
            }
            printf("Message (20 max) : ");
            viderBuffer();
            if(scanf("%20s",message)!=1){
                printf("Problème avec saisie du message\n");
                return EXIT_FAILURE;
            }
            newMsg.type=TYPE_CLIENT;
            newMsg.id=newRegister.id;/*id du trieur pas pidd*/
            newMsg.dest=dest;
            strcpy(newMsg.chaine,message);
            if(msgsnd(msqid, &newMsg, sizeof(msgClient_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi du message ");
                exit(EXIT_FAILURE);
            }
            printf("Client %d : message envoyé au client %d.\n",newRegister.id, newMsg.dest);
        }
        
        if(ordre==2){
            /*Checking des messages reçus*/
            printf("Client : verification des msg recu...\n");
            if(msgrcv(msqid, &msgRecu, sizeof(requete_t) - sizeof(long), pidd, 0) == -1) {
                perror("Erreur lors de la lecture d'un msg ");
                exit(EXIT_FAILURE);
            }
        }

        if (ordre == 1) {
            /*Demande d'arret*/
            if(msgsnd(msqid, &newRegister, sizeof(enregistre_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi de la demande d'arret ");
                exit(EXIT_FAILURE);
            }
            printf("Client %d : demande d'arret envoyee.\n",pidd);
            
            enCours = 0;
        }
    }
   

    return EXIT_SUCCESS;
}