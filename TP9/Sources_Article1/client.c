/**
 * Ce programme permet d'envoyer un message au serveur via une socket UDP. 
 * L'adresse IP et le port du serveur sont passés en argument du programme.
 * @author Cyril Rabat
 **/
#include <stdlib.h>      /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>       /* Pour printf, fprintf, perror */
#include <sys/socket.h>  /* Pour socket */
#include <arpa/inet.h>   /* Pour sockaddr_in, inet_pton */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */

#include "include.h"

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in adresseServeur;
  char msg[TAILLE_MAX];
  
  /* Vérification des arguments */
  if(argc != 4) {
    fprintf(stderr, "Usage : %s adresseServeur portServeur message\n", argv[0]);
    fprintf(stderr, "Où :\n");
    fprintf(stderr, "  adresseServeur : adresse IPv4 du serveur\n");
    fprintf(stderr, "  portServeur    : numéro de port du serveur\n");
    fprintf(stderr, "  message        : le message à envoyer\n");
    exit(EXIT_FAILURE);
  }
  
  /* Vérification de la taille du message */
  if(strlen(argv[3]) >= TAILLE_MAX) {
      fprintf(stderr, "La taille du message est trop grande. Elle doit fait maximum %d caractères.", TAILLE_MAX);
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
  adresseServeur.sin_port = htons(atoi(argv[2]));
  if(inet_pton(AF_INET, argv[1], &adresseServeur.sin_addr) != 1) {
    perror("Erreur lors de la conversion de l'adresse ");
    exit(EXIT_FAILURE);
  }

  /* Préparation du message */
  memset(msg, 0, TAILLE_MAX);
  strcpy(msg, argv[3]);

  /* Envoi du message */
  if(sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&adresseServeur, sizeof(struct sockaddr_in)) == -1) {
    perror("Erreur lors de l'envoi du message ");
    exit(EXIT_FAILURE);
  }
  printf("Client : message envoyé.\n");

  /* Fermeture de la socket */
  if(close(sockfd) == -1) {
    perror("Erreur lors de la fermeture de la socket ");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
