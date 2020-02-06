/*
Le trieur créer la file de msg
Les client s'enregistre auprès du trieur si il existe deja on l'arrete (en le retirant des clients)
    envoie d'un message avec TYPE_ENREGISTRE et son id par le client
Le client envoie un msg avec son id et celui du destinataire du msg
    envoie d'un message avec TYPE_CLIENT par le client
Le trieur vérifie que le client qui envoie et celui qui doit recevoir sont enregistres
    Il renvoie le msg au destinataire avec TYPE_TRIEUR

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

#include "struct.h"

int main(int argc, char* argv[]) {
    int i, indice, trouve, msqid, cle, clients[MAX_CLIENT] = {-1};
    long type;
    enregistre_t newClient;
    msgClient_t newMsg;

    /* Création de la file si elle n'existe pas */
    if((msqid = msgget((key_t)cle, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST){
            /* Recuperation de la file */
            if((msqid = msgget((key_t)cle, 0)) == -1) {
                if(errno == ENOENT)
                    fprintf(stderr, "Aucune file présente avec la clé %d.\n", (key_t)cle);
                else
                    perror("Erreur lors de la récupération de la file ");
                exit(EXIT_FAILURE);
            }
        }
        else{
            perror("Erreur lors de la creation de la file ");
            exit(EXIT_FAILURE);
        }
    }


    /* *************** Attente d'une requête: TYPE_ENREGISTRE *************** */
    printf("Serveur : en attente d'une requête...\n");
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
        indice = clients[i];
        while (indice != -1) {
            indice = clients[i];
            i++;
        }
        if (i<MAX_CLIENT) clients[i] = newClient.id;
        else printf("Le nombre MAX de clients est atteint\n");
    }
    

    /* *************** Attente d'un msg: TYPE_CLIENT *************** */
    printf("Serveur : en attente d'un msg...\n");
    if(msgrcv(msqid, &newMsg, sizeof(enregistre_t) - sizeof(long), TYPE_CLIENT, 0) == -1) {
        perror("Erreur lors de la réception d'un msg ");
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

    }



    return EXIT_SUCCESS;
}