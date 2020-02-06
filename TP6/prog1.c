#define _XOPEN_SOURCE
#define SUP 0
#define ENV 1
#define REC 2

#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

#include "struct.h"

int main(int argc, char * argv []) {
    int msqid, cle, ordre;
    long id=1;
    char /*ordre [3],*/chaine [20];
    requete_t requeteE, requeteR;

    
    if(argc==3){
        /*Cas de la suppression*/
        if(strcmp(argv[2],"-S")!=0){
            printf("Arguments incorrects pour la supression\n clé et -S attendus\n");
            return(EXIT_FAILURE);        
        }
        /*memcpy(ordre,argv[2],strlen(argv[2])+1);*/
        ordre = SUP;
    }
    else{
        if(argc==4){
            /*Cas de la réception*/
            if(strcmp(argv[3],"-R")!=0){
                printf("Arguments incorrects pour la reception\n clé,identifiant et -R attendus\n");
                return(EXIT_FAILURE);        
            }
            id=atoi(argv[2]);
            /*memcpy(ordre,argv[3],strlen(argv[3])+1);*/
            ordre = REC;
        }
        else{
            if(argc==5){
                /*Cas de l'envoi*/
                if(strcmp(argv[3],"-E")!=0){
                    printf("Arguments incorrects pour l'envoi'\n clé,identifiant, -E et chaine attendus\n");
                    return(EXIT_FAILURE);        
                }
                id=atoi(argv[2]);
                /*memcpy(ordre,argv[3],strlen(argv[3])+1);*/
                ordre = ENV;
                memcpy(chaine,argv[4],strlen(argv[4])+1);
                if(strlen(chaine)>CHAINE_MAX){
                    printf("Entrez une nouvelle chaine (max 20 caractères):\n ");
                    if(scanf("%20s",chaine)!=1){
                        printf("Saisie de la chaine échouée\n");
                        return EXIT_FAILURE;
                    }
                }
            }
            else{
                /*Cas du fail*/
                printf("Pas assez d'arguments\n\t1: clé de la file de messages\n\t");
                printf("2: ordre -R(eception) -E(nvoi) -S(upprimer)\n\t3: la chaine de caractères (max 20)\n");
                return(EXIT_FAILURE);
            }
        }
    }
    cle=atoi(argv[1]);

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

    switch(ordre) {
        case REC:
            /* Attente d'une requête */
            printf("Serveur : en attente d'une requête...\n");
            if(msgrcv(msqid, &requeteR, sizeof(requete_t) - sizeof(long), id, 0) == -1) {
                perror("Erreur lors de la réception d'une requête ");
                exit(EXIT_FAILURE);
            }
            printf("Programme : chaine reçue\n %s\n", requeteR.chaine);
            break;
        case ENV:
            memcpy(requeteE.chaine,chaine,strlen(chaine)+1);
            requeteE.type=id;
            if(msgsnd(msqid, &requeteE, sizeof(requete_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi de la requete ");
                exit(EXIT_FAILURE);
            }
            printf("Programme : requete envoyée.\n");
            break;
        case SUP:
            /* Suppression de la file */
            if(msgctl(msqid, IPC_RMID, 0) == -1) {
                perror("Erreur lors de la suppression de la file ");
                exit(EXIT_FAILURE);
            }
            printf("File supprimée.\n");
            break;
        default:
            printf("Ordre non correct\n");
            exit(EXIT_FAILURE);
            break;
    }
/*
    if(!strcmp(ordre,"-R")){
        // Attente d'une requête 
        printf("Serveur : en attente d'une requête...\n");
        if(msgrcv(msqid, &requeteR, sizeof(requete_t) - sizeof(long), id, 0) == -1) {
            perror("Erreur lors de la réception d'une requête ");
            exit(EXIT_FAILURE);
        }
        printf("Programme : chaine reçue\n %s\n", requeteR.chaine);
    }
    else{
        if(!strcmp(ordre,"-E")){
            memcpy(requeteE.chaine,chaine,strlen(chaine)+1);
            requeteE.type=id;
            if(msgsnd(msqid, &requeteE, sizeof(requete_t) - sizeof(long), 0) == -1) {
                perror("Erreur lors de l'envoi de la requete ");
                exit(EXIT_FAILURE);
            }
            printf("Programme : requete envoyée.\n");
        }
        else{
            if(!strcmp(ordre,"-S")){
                // Suppression de la file 
                if(msgctl(msqid, IPC_RMID, 0) == -1) {
                    perror("Erreur lors de la suppression de la file ");
                    exit(EXIT_FAILURE);
                }
                printf("File supprimée.\n");
            }
            else{
                printf("Ordre non correct\n");
                exit(EXIT_FAILURE);
            }
        }
    }
*/
    return EXIT_SUCCESS;
}
