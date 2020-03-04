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
  struct sockaddr_in adresseClient;
  socklen_t taille;
  char msg[TAILLE_MAX];
  char stock [TAILLE_STOCK][TAILLE_MAX];
  int tete=0,queue=0,nb=0;

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
  while(1){
    /* Attente de la requête du client */
    printf("Serveur en attente du message du client.\n");
    taille=sizeof(struct sockaddr_in);
    if(recvfrom(sockfd, &msg, sizeof(msg), 0,(struct sockaddr*)&adresseClient, &taille) == -1) {
      perror("Erreur lors de la réception du message ");
      exit(EXIT_FAILURE);
    }
    printf("Serveur : message recu '%s'.\n", msg);

    if(!strcmp(msg,"-g")){
      /* Préparation du message */
      memset(msg, 0, TAILLE_MAX);
      if(nb==0){
        /*pas de messages disponibles*/
        strcpy(msg,"");
      }
      else {
        strcpy(msg,stock[tete]);
        tete++;
        tete=tete%TAILLE_STOCK;
        nb--;
      }
      /* Envoi du message */
      if(sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&adresseClient, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
      }
      printf("Serveur : message envoyé.\n");
    }
    else{
      if(nb==TAILLE_STOCK){
        fprintf(stderr,"La file est pleine\n");
      }
      else{
        strcpy(stock[queue],msg);
        queue++;
        queue=queue%TAILLE_STOCK;
        nb++;
      }
    }
  }
  

  /* Fermeture de la socket */
  if(close(sockfd) == -1) {
    perror("Erreur lors de la fermeture de la socket ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur terminé.\n");
  
  return EXIT_SUCCESS;
}
