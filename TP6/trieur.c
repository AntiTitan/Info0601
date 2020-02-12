/*
Le trieur créé la file de msg
Les client s'enregistre auprès du trieur si il existe deja on l'arrete (en le retirant des clients)
    envoie d'un message avec TYPE_ENREGISTRE et son id par le client
Le client envoie un msg avec son id et celui du destinataire du msg
    envoie d'un message avec TYPE_CLIENT par le client
Le trieur vérifie que le client qui envoie et celui qui doit recevoir sont enregistres
    Il renvoie le msg au destinataire avec pour type l'id (pid) du destinataire

Pour l'enregistrement, s'il est accepté, le trieur envoie au client
son id, ou -1 s'il n'y a plus de place

tableau statique de taille MAX_CLIENT
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

void initZero(enregistre_t * newClient, msgClient_t * newMsg, requete_t * newReq){
    newClient->type=0;
    newMsg->type=0;
    newReq->type=0;
}

int main(int argc, char* argv[]) {
    int i, indice, trouve, msqid;
    pid_t clients[MAX_CLIENT] = {-1};
   /* long type,choix; */
    enregistre_t newClient;
    msgClient_t newMsg;
    requete_t newReq;
    initZero(&newClient,&newMsg,&newReq);
    /*
      On init tous les types à 0
      Une fois qu'on a tenté de récup chaque type de message,
      On fait un switch, si un des message a un type != de 0,
      On traite son cas, puis on reboucle
    */

    /* Création de la file si elle n'existe pas */
    if((msqid = msgget((key_t)CLE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST){
            /* Recuperation de la file */
            if((msqid = msgget((key_t)CLE, 0)) == -1) {
                if(errno == ENOENT)
                    fprintf(stderr, "Aucune file présente avec la clé %d.\n", (key_t)CLE);
                else
                    perror("Erreur lors de la récupération de la file (trieur)");
                exit(EXIT_FAILURE);
            }
        }
        else{
            perror("Erreur lors de la creation de la file ");
            exit(EXIT_FAILURE);
        }
    }

    while(1){

        /* *************** Gestion d'une requête: TYPE_ENREGISTRE *************** */
        printf("Trieur : en attente d'une requête...\n");
        if(msgrcv(msqid, &newClient, sizeof(enregistre_t) - sizeof(long), TYPE_ENREGISTRE, 0) == -1) {
            perror("Erreur lors de la réception d'une requête ");
            exit(EXIT_FAILURE);
        }

        /* Vérification de l'existence du client (pour savoir si on l'enregistre ou si on l'arete) */
        trouve = 0;
        indice = MAX_CLIENT;
        for (i=0; i<MAX_CLIENT; i++) {
            if (clients[i] == newClient.id) {
                indice = i;
                trouve = 1;
            }
        }
        /* Le client existe: il demande l'aret */
        if (trouve == 1) clients[indice] = -1;
        /* Le client n'existe pas: il demande l'enregistrement */
        else {
            i = 0;
            indice = 0;
            while (indice != -1) {
                indice = clients[i];
                printf("client[%d] = %d\n", i, indice);
                i++;
            }
            if (i<MAX_CLIENT) {
                clients[i] = newClient.id;
                printf("Client %d enregistré\n", newClient.id);
                newClient.type = newClient.id;
                newClient.id = i;
            }
            else { /*Plus de place pour un nouveau client*/
                newClient.type = newClient.id;
                newClient.id = -1;
                printf("Le nombre MAX de clients est atteint\n");
            }
            /*Envoie de la confirmation ou du refus pour manque de place*/
            if(msgsnd(msqid, &newClient, sizeof(enregistre_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi de la requete ");
                exit(EXIT_FAILURE);
            }

        }

        /* *************** Gestion d'un msg: TYPE_CLIENT *************** */
        printf("Trieur : verification de la reception des msg...\n");
        if(msgrcv(msqid, &newMsg, sizeof(msgClient_t) - sizeof(long), TYPE_CLIENT, 0) == -1) {
            perror("Erreur lors de la réception d'un msg (trieur)");
            exit(EXIT_FAILURE);
        }

        /* Vérification de l'existence du destinataire */
        trouve = 0;
        indice = MAX_CLIENT;
        for (i=0; i<MAX_CLIENT; i++) {
            if (clients[i] == newMsg.dest) {
                indice = i;
                trouve = 1;
            }
        }
        /* Le client existe: le trieur lui envoie le msg*/
        if (trouve == 1) {
            newReq.type = newMsg.dest;
            strcpy(newReq.chaine, newMsg.chaine);
            if(msgsnd(msqid, &newReq, sizeof(requete_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoie d'un msg depuis le trieur ");
                exit(EXIT_FAILURE);
            }
        }
        else printf("Le client %d n'exite pas!!!", newMsg.dest);
        /* *************** Gestion des messages reçues *************** */
        /*if(newClient.type!=0){

        }
        if(newMsg.type!=0){

        }*/
    }
    return EXIT_SUCCESS;
}
