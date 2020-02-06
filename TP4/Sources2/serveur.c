/**
 * Ce programme illustre l'utilisation d'un tube nommé. Le programme crée un
 * tube nommé et y écrit 5 entiers. Ensuite, ceux-ci sont lus par le programme
 * 'client'.
 * @author Cyril Rabat
 * @version 21/01/2020
 */
#include <stdlib.h>   /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>    /* Pour printf, perror */
#include <unistd.h>   /* Pour unlink, write, close, sleep */
#include <fcntl.h>    /* Pour open */
#include <sys/stat.h> /* Pour mkfifo */
#include <errno.h>    /* Pour errno */

#include "include.h"

int main(int argc, char *argv[]) {
  int fd, i;

  /* Création du tube */
  if(mkfifo(NOM_TUBE, S_IRUSR | S_IWUSR) == -1) {
    if(errno != EEXIST) {
      fprintf(stderr, "Erreur lors de la création du tube '%s'", NOM_TUBE);
      perror(" ");
      exit(EXIT_FAILURE);
    }
    else
      fprintf(stderr, "Le tube '%s' existe déjà.\n", NOM_TUBE);
  }

  /* Ouverture du tube */
  if((fd = open(NOM_TUBE, O_WRONLY)) == -1) {
    fprintf(stderr, "Erreur lors de l'ouverture du tube '%s'", NOM_TUBE);
    perror(" ");
    exit(EXIT_FAILURE);
  }
  printf("Serveur prêt.\n");
  sleep(1);
  
  /* Ecriture de 5 entiers */
  for(i = 0; i < 5; i++) {
    if(write(fd, &i, sizeof(int)) == -1) {
      perror("Erreur lors de l'écriture d'un entier dans le tube ");
      exit(EXIT_FAILURE);
    }
  }
  printf("Serveur, envoyé : ");
  for(i = 0; i < 5; i++)
    printf("%d ", i);
  printf("\n");  

  /* Fermeture du tube */
  if(close(fd) == -1) {
    perror("Erreur lors de la fermeture du tube ");
    exit(EXIT_FAILURE);
  }
  
  /* Suppression du tube */
  if(unlink(NOM_TUBE) == -1) {
    if(errno != ENOENT) {
      fprintf(stderr, "Erreur lors de la suppression du tube '%s'", NOM_TUBE);
      perror(" ");
      exit(EXIT_FAILURE);
    }
  }  

  printf("Serveur terminé.\n");

  return EXIT_SUCCESS;
}
