/**
 * Ce programme permet d'envoyer un message au serveur via une socket TCP. 
 * Le message, passé en argument du programme, est envoyé en deux temps : 
 * sa taille, puis le contenu du message. L'adresse IP et le port du serveur
 * sont passés en arguments du programme.
 * @author Cyril Rabat
 **/
#define _XOPEN_SOURCE
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/socket.h> /* Pour socket */
#include <arpa/inet.h>  /* Pour IPPROTO_TCP */
#include <stdio.h>      /* Pour perror */
#include <unistd.h>     /* Pour close */
#include <string.h>     /* Pour memset */
#include <signal.h>     /* Pour sigaction */
#include <sys/types.h>  /*Pour kill*/
#define TAILLE_MAX 20

int fin = 0;
void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

int main(int argc, char *argv[]) {
  int fd;
  struct sockaddr_in adresse;
  size_t taille;
  char  msg[TAILLE_MAX];
  char *msgR;
 
  /* Vérification des arguments */
  if(argc != 3) {
    fprintf(stderr, "Usage : %s adresseServeur portServeur message\n", argv[0]);
    fprintf(stderr, "Où :\n");
    fprintf(stderr, "  adresseServeur : adresse IPv4 du serveur\n");
    fprintf(stderr, "  portServeur    : numéro de port du serveur\n");
    exit(EXIT_FAILURE);
  }

  /* Positionnement du gestionnaire pour SIGINT */
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if(sigaction(SIGINT, &action, NULL) == -1) {
    fprintf(stderr, "Erreur lors du positionnement\n");
    exit(EXIT_FAILURE);
  }

  /* Création de la socket */
  if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("Erreur lors de la création de la socket ");
    exit(EXIT_FAILURE);
  }

  /* Remplissage de la structure */
  memset(&adresse, 0, sizeof(struct sockaddr_in));
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons(atoi(argv[2]));
  if(inet_pton(AF_INET, argv[1], &adresse.sin_addr.s_addr) != 1) {
    perror("Erreur lors de la conversion de l'adresse ");
    exit(EXIT_FAILURE);
  }

  /* Connexion au serveur */
  if(connect(fd, (struct sockaddr*)&adresse, sizeof(adresse)) == -1) {
    perror("Erreur lors de la connexion ");
    exit(EXIT_FAILURE);
  }

  while(!fin){
    printf("Message à envoyer : ");
    if(scanf("%[^\n]",msg)==0){ /*thread lecture msg utilisateur*/
      exit(EXIT_FAILURE);
    }
    viderBuffer();

    if(strcmp(msg,"Au revoir")==0){
      fin =1;
    }

    /*vidage du buffer*/
    
    /* Envoi du message au serveur */
    taille = strlen(msg) + 1;
    if(write(fd, &taille, sizeof(size_t)) == -1) {
      perror("Erreur lors de l'envoi de la taille du message ");
      exit(EXIT_FAILURE);
    }
    if(write(fd, msg, sizeof(char) * taille) == -1) {
      perror("Erreur lors de l'envoi du message ");
      exit(EXIT_FAILURE);
    }
    printf("Client : message envoyé.\n");

    /*Lecture du message */
    if(read(fd, &taille, sizeof(size_t)) == -1) {
      perror("Erreur lors de la lecture de la taille du message ");
      exit(EXIT_FAILURE);
    }
    if((msgR = (char*)malloc(sizeof(char) * taille)) == NULL) {
      perror("Erreur lors de l'allocation mémoire pour le message ");
      exit(EXIT_FAILURE);
    }
    if(read(fd, msgR, sizeof(char) * taille) == -1) {
      perror("Erreur lors de la lecture de la taille du message ");
      exit(EXIT_FAILURE);
    }
    printf("Client : message recu '%s'.\n", msgR);
  }

  /* Fermeture de la socket */
  if(close(fd) == -1) {
    perror("Erreur lors de la fermeture de la socket ");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
