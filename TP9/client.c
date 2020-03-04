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
  if(argc < 4) {
    fprintf(stderr, "Usage : %s adresseServeur portServeur commande\n", argv[0]);
    fprintf(stderr, "Où :\n");
    fprintf(stderr, "  adresseServeur : adresse IPv4 du serveur\n");
    fprintf(stderr, "  portServeur    : numéro de port du serveur\n");
    fprintf(stderr, "  commande       : -s ->envoyer msg, -g -> recevoir msg\n");
    exit(EXIT_FAILURE);
  }
  if(!strcmp(argv[3],"-s") && argc != 5){
    fprintf(stderr, "Vous devez rajouter un message\n");
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

  if(!strcmp(argv[3],"-s")){
    strcpy(msg, argv[4]);
  }
  else if(!strcmp(argv[3],"-g")){
    strcpy(msg, "-g");
  }
  
  /* Envoi du message */
  if(sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&adresseServeur, sizeof(struct sockaddr_in)) == -1) {
    perror("Erreur lors de l'envoi du message ");
    exit(EXIT_FAILURE);
  }
  printf("Client : message envoyé.\n");

  if(!strcmp(argv[3],"-g")){
    /* Attente de la reponse du serveur */
    printf("Client en attente du message du serveur.\n");
    if(recvfrom(sockfd, &msg, sizeof(msg), 0, NULL, NULL) == -1) {
      perror("Erreur lors de la réception du message ");
      exit(EXIT_FAILURE);
    }
    if(!strcmp(msg,"")){
      printf("Client : plus de messages dans la file.\n");
    }
    else{
      printf("Client : message recu '%s'.\n", msg);
    }
    
  }

  /* Fermeture de la socket */
  if(close(sockfd) == -1) {
    perror("Erreur lors de la fermeture de la socket ");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
