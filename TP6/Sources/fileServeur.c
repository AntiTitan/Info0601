/**
 * Ce programme vise à illustrer le fonctionnement d'une file de messages
 * IPC System V. Le serveur crée une file de messages et se met en attente
 * d'une requête. Des qu'il en reçoit une, il envoie une réponse.
 * @author Cyril Rabat
 **/
#define _XOPEN_SOURCE

#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

#include "structures.h"

int main() {
  int msqid;
  requete_t requete;
  reponse_t reponse;

  /* Création de la file si elle n'existe pas */
  if((msqid = msgget((key_t)CLE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST)
      fprintf(stderr, "Erreur : file (cle=%d) existante\n", CLE);
    else
      perror("Erreur lors de la creation de la file ");
    exit(EXIT_FAILURE);
  }

  /* Attente d'une requête */
  printf("Serveur : en attente d'une requête...\n");
  if(msgrcv(msqid, &requete, sizeof(requete_t) - sizeof(long), TYPE_REQUETE, 0) == -1) {
    perror("Erreur lors de la réception d'une requête ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur : requête reçue (%d, %d)\n", requete.valeur1, requete.valeur2);

  /* Envoi de la réponse */
  reponse.type = TYPE_REPONSE;
  reponse.resultat = requete.valeur1 + requete.valeur2;

  if(msgsnd(msqid, &reponse, sizeof(reponse_t) - sizeof(long), 0) == -1) {
    perror("Erreur lors de l'envoi de la reponse ");
    exit(EXIT_FAILURE);
  }
  printf("Serveur : réponse envoyée.\n");

  return EXIT_SUCCESS;
}
