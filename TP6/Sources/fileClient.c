/**
 * Ce programme vise à illustrer le fonctionnement d'une file de messages
 * IPC System V. Le client envoie une requête dans la file de messages et se
 * met en attente d'une réponse.
 * @author Cyril Rabat
 **/
#define _XOPEN_SOURCE

#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */

#include "structures.h"

int main() {
  int msqid;
  requete_t requete;
  reponse_t reponse;

  /* Récupération de la file */
  if((msqid = msgget((key_t)CLE, 0)) == -1) {
    perror("Erreur lors de la récupération de la file ");
    exit(EXIT_FAILURE);
  }

  /* Envoi d'une requête */
  requete.type = TYPE_REQUETE;
  requete.valeur1 = 3;
  requete.valeur2 = 6;

  if(msgsnd(msqid, &requete, sizeof(requete_t) - sizeof(long), 0) == -1) {
    perror("Erreur lors de l'envoi de la requête ");
    exit(EXIT_FAILURE);
  }
  printf("Client : envoi d'une requête.\n");

  /* Réception de la réponse */
  printf("Client : attente de la reponse...\n");
  if(msgrcv(msqid, &reponse, sizeof(reponse_t) - sizeof(long), TYPE_REPONSE, 0) == -1) {
    perror("Erreur lors de la réception de la réponse ");
    exit(EXIT_FAILURE);    
  }

  printf("Client : le resultat est %d\n", reponse.resultat);
  
  return EXIT_SUCCESS;
}

