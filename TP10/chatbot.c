/**
 * Ce programme se met en attente de connexion de la part d'un client. Une fois
 * la connexion établie, il se met en attente de la réception d'un message qui
 * est envoyé en deux temps : d'abord la taille, puis le contenu du message. 
 * Ce programme s'arrête dès la réception du message.
 * @author Antinéa Loiseau--Pintaux || Mina Warnet
 **/
#define _XOPEN_SOURCE
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/socket.h> /* Pour socket */
#include <arpa/inet.h>  /* Pour sockaddr_in, IPPROTO_TCP */
#include <stdio.h>      /* Pour perror */
#include <unistd.h>     /* Pour close */
#include <string.h>     /* Pour memset */
#include <signal.h>     /* Pour sigaction */
#include <sys/types.h>  /*Pour kill*/
#include <time.h>
#define MAX_CLIENT 5

int fin=0;
pid_t pid[MAX_CLIENT];

void handler(int sig){
  int i;

  if(sig==SIGINT){
    for(i=0; i<MAX_CLIENT; i++) {
      kill(pid[i], SIGINT);
    }
    fin=1;
  }
  printf("Fin handler\n");
  exit(EXIT_SUCCESS);
}

void fils(int sockclient) {
  int fin=0,alea;
  size_t taille,tailleRep;
  char * rep [3] = {"Je ne vous le fait pas dire!","Absolument.","Je ne crois pas non!"};
  char * msg;

  srand(time(NULL)+getpid());

  while(!fin) {
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
    printf("Fils %d : message recu '%s'.\n", getpid(), msg);

    if(strcmp(msg,"Au revoir")==0){
      /* Envoi du message de fin au client */
      fin = 1;
      tailleRep=strlen("Au revoir");
      if(write(sockclient, &tailleRep, sizeof(size_t)) == -1) {
        perror("Erreur lors de l'envoi de la taille du message ");
        exit(EXIT_FAILURE);
      }
      if(write(sockclient, "Au revoir", sizeof(char) * tailleRep) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
      }
      printf("fils %d : message de fin envoyé.\n", getpid());
    }

    else{
      /* Envoi du message au client */
      alea=rand()%3;
      tailleRep = strlen(rep[alea]) + 1;

      if(write(sockclient, &tailleRep, sizeof(size_t)) == -1) {
        perror("Erreur lors de l'envoi de la taille du message ");
        exit(EXIT_FAILURE);
      }
      if(write(sockclient, rep[alea], sizeof(char) * tailleRep) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
      }
      printf("Fils %d : message %d envoyé.\n", getpid(), alea);
    }
  }

  /* Liberation mémoire */
  free(msg);
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  int fd, i, sockclient[MAX_CLIENT]={0,0,0,0,0},j,trouve=0;
  struct sockaddr_in adresse;
  struct sigaction action;

  /* Vérification des arguments */
  if(argc != 2) {
    fprintf(stderr, "Usage : %s port\n", argv[0]);
    fprintf(stderr, "Où :\n");
    fprintf(stderr, "  port : le numéro de port d'écoute du serveur\n");
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
  while(!fin){
    j=0;
    trouve=0;
    while(!trouve && j<5){
      if(sockclient[j]!=0){
        j++;
      }
      else{
        trouve=1;
      }
    }
    if(trouve==1){
      /* Attente d'une connexion */
      printf("Serveur : attente de connexion...\n");
      if((sockclient[j] = accept(fd, NULL, NULL)) == -1) {
        perror("Erreur lors de la demande de connexion ");
        exit(EXIT_FAILURE);
      }
      if((pid[j] = fork()) == -1) {
        perror("Père: erreur lors de la création du fils");
        exit(EXIT_FAILURE);
      }
      if(pid[j] == 0) {
        fils(sockclient[j]);
      }
      
    }
  }
  

  /* Fermeture des sockets */
  for(i=0;i<MAX_CLIENT;i++){
    if(close(sockclient[i]) == -1) {
      perror("Erreur lors de la fermeture de la socket de communication ");
      exit(EXIT_FAILURE);
    }
  }
  
  if(close(fd) == -1) {
    perror("Erreur lors de la fermeture de la socket de connexion ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur terminé.\n");

  return EXIT_SUCCESS;
}
