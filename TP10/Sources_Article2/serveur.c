/**
 * Ce programme se met en attente de connexion de la part d'un client. Une fois
 * la connexion établie, il se met en attente de la réception d'un message qui
 * est envoyé en deux temps : d'abord la taille, puis le contenu du message. 
 * Ce programme s'arrête dès la réception du message.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/socket.h> /* Pour socket */
#include <arpa/inet.h>  /* Pour sockaddr_in, IPPROTO_TCP */
#include <stdio.h>      /* Pour perror */
#include <unistd.h>     /* Pour close */
#include <string.h>     /* Pour memset */

int main(int argc, char *argv[]) {
  int fd, sockclient;
  struct sockaddr_in adresse;
  size_t taille;
  char *msg;

  /* Vérification des arguments */
  if(argc != 2) {
    fprintf(stderr, "Usage : %s port\n", argv[0]);
    fprintf(stderr, "Où :\n");
    fprintf(stderr, "  port : le numéro de port d'écoute du serveur\n");
    exit(EXIT_FAILURE);
  }

  /* Création de la socket */
  if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("Erreur lors de la création de la socket ");
    exit(EXIT_FAILURE);
  }

  /* Création de l'adresse du serveur */
  memset(&adresse, 0, sizeof(struct sockaddr_in));
  adresse.sin_family = AF_INET;
  adresse.sin_addr.s_addr = htonl(INADDR_ANY);
  adresse.sin_port = htons(atoi(argv[1]));

  /* Nommage de la socket */
  if(bind(fd, (struct sockaddr*)&adresse, sizeof(struct sockaddr_in)) == -1) {
    perror("Erreur lors du nommage de la socket ");
    exit(EXIT_FAILURE);
  }

  /* Mise en mode passif de la socket */
  if(listen(fd, 1) == -1) {
    perror("Erreur lors de la mise en mode passif ");
    exit(EXIT_FAILURE);
  }

  /* Attente d'une connexion */
  printf("Serveur : attente de connexion...\n");
  if((sockclient = accept(fd, NULL, NULL)) == -1) {
    perror("Erreur lors de la demande de connexion ");
    exit(EXIT_FAILURE);
  }

  /* Lecture du message */
  if(read(sockclient, &taille, sizeof(size_t)) == -1) {
    perror("Erreur lors de la lecture de la taille du message ");
    exit(EXIT_FAILURE);
  }
  if((msg = (char*)malloc(sizeof(char) * taille)) == NULL) {
    perror("Erreur lors de l'allocation mémoire pour le message ");
    exit(EXIT_FAILURE);
  }
  if(read(sockclient, msg, sizeof(char) * taille) == -1) {
    perror("Erreur lors de la lecture de la taille du message ");
    exit(EXIT_FAILURE);
  }
  printf("Serveur : message recu '%s'.\n", msg);

  /* Fermeture des sockets */
  if(close(sockclient) == -1) {
    perror("Erreur lors de la fermeture de la socket de communication ");
    exit(EXIT_FAILURE);
  }
  if(close(fd) == -1) {
    perror("Erreur lors de la fermeture de la socket de connexion ");
    exit(EXIT_FAILURE);
  }

  /* Liberation mémoire */
  free(msg);

  printf("Serveur terminé.\n");

  return EXIT_SUCCESS;
}
