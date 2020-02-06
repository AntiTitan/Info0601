/**
 * Ce programme illustre l'utilisation d'un tube nommé. Celui-ci est crée par
 * le programme 'serveur' qui envoie 5 entiers. Le client lit ces 5 entiers et
 * les affiche à l'écran.
 * @author Cyril Rabat
 * @version 21/01/2020
 */
#include <stdlib.h>   /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>    /* Pour printf, perror */
#include <unistd.h>   /* Pour read, close, sleep */
#include <fcntl.h>    /* Pour open */

#include "include.h"

int main(int argc, char *argv[]) {
  int fd, tab[5], i;

  /* Ouverture du tube */
  if((fd = open(NOM_TUBE, O_RDONLY)) == -1) {
    perror("Erreur lors de l'ouverture du tube ");
    exit(EXIT_FAILURE);
  }
  printf("Client prêt.\n");

  /* Lecture de 5 entiers */
  if(read(fd, tab, sizeof(int) * 5) == -1) {
    perror("Erreur lors de la lecture de 5 entiers dans le tube ");
    exit(EXIT_FAILURE);
  }
  printf("Client, lu : ");
  for(i = 0; i < 5; i++)
    printf("%d ", i);
  printf("\n");  

  /* Fermeture du tube */
  if(close(fd) == -1) {
    perror("Erreur lors de la fermeture du tube ");
    exit(EXIT_FAILURE);
  }

  printf("Client terminé.\n");

  return EXIT_SUCCESS;
}
