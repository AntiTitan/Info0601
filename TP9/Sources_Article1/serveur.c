/**
 * Ce programme se met en attente d'un message de la part d'un client, via une
 * socket UDP. Ce programme s'arrête dès la réception du message.
 * @author Cyril Rabat
 **/
#include <stdlib.h>      /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>       /* Pour printf, fprintf, perror */
#include <sys/socket.h>  /* Pour socket, bind */
#include <arpa/inet.h>   /* Pour sockaddr_in */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */

#include "include.h"

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in adresseServeur;
  char msg[TAILLE_MAX];

  /* Récupération des arguments */
  if(argc != 2) {
    fprintf(stderr, "Usage : %s port\n", argv[0]);
    fprintf(stderr, "Où :\n");
    fprintf(stderr, "  port           : port d'écoute du serveur\n");
    exit(EXIT_FAILURE);
  }

  /* Création de la socket */
  if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("Erreur lors de la création de la socket ");
    exit(EXIT_FAILURE);
  }

  /* Création de l'adresse du serveur */
  memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
  adresseServeur.sin_family = AF_INET;
  adresseServeur.sin_port = htons(atoi(argv[1]));
  adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY);

  /* Nommage de la socket */
  if(bind(sockfd, (struct sockaddr*)&adresseServeur, sizeof(struct sockaddr_in)) == -1) {
    perror("Erreur lors du nommage de la socket ");
    exit(EXIT_FAILURE);
  }

  /* Attente de la requête du client */
  printf("Serveur en attente du message du client.\n");
  if(recvfrom(sockfd, &msg, sizeof(msg), 0, NULL, NULL) == -1) {
    perror("Erreur lors de la réception du message ");
    exit(EXIT_FAILURE);
  }
  printf("Serveur : message recu '%s'.\n", msg);

  /* Fermeture de la socket */
  if(close(sockfd) == -1) {
    perror("Erreur lors de la fermeture de la socket ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur terminé.\n");
  
  return EXIT_SUCCESS;
}
