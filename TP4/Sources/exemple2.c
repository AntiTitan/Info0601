/**
 * Ce programme montre comment créer deux tubes anonymes entre un processus pere
 * et un processus fils pour communiquer dans les deux sens. Ils sont utilisés
 * pour envoyer des entiers de l'un vers l'autre et inversement.
 * @author Cyril Rabat
 * @version 21/01/2020
 */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf, perror */
#include <unistd.h>    /* Pour fork, pipe */
#include <sys/wait.h>  /* Pour waitpid */
#include <sys/types.h> /* Pour pid_t */

#define TUBE_LECTURE  0
#define TUBE_ECRITURE 1

void fils(int p_f[2], int f_p[2]) {
  int i, tmp;

  /* Fermeture des descripteurs inutiles */
  if((close(p_f[TUBE_ECRITURE]) == -1) ||
     (close(f_p[TUBE_LECTURE]) == -1)) {
    perror("Fils : erreur lors de la fermeture des descripteurs ");
    exit(EXIT_FAILURE);
  }

  /* Lecture et envoi de 5 entiers */
  for(i = 0; i < 5; i++) {
    if(read(p_f[TUBE_LECTURE], &tmp, sizeof(int)) == -1) {
      perror("Fils : erreur lors de la lecture d'un entier depuis le tube ");
      exit(EXIT_FAILURE);
    }
    printf("Fils : entier lu : %d\n", tmp);
    tmp *= 2;
    if(write(f_p[TUBE_ECRITURE], &tmp, sizeof(int)) == -1) {
      perror("Fils : erreur lors de l'écriture d'un entier dans le tube ");
      exit(EXIT_FAILURE);
    }
    printf("Fils : entier envoyé : %d\n", tmp);
    sleep(1);
  }

  /* Fermeture des descripteurs */
  if((close(p_f[TUBE_LECTURE]) == -1) ||
     (close(f_p[TUBE_ECRITURE]))) {
    perror("Fils : erreur lors de la fermeture des descripteurs ");
    exit(EXIT_FAILURE);
  }
  
  printf("Fils : terminé.\n");
  
  exit(EXIT_SUCCESS);
}

int main() {
  pid_t pid;
  int p_f[2], f_p[2], i, tmp;

  /* Création des tubes */
  if((pipe(p_f) == -1) || (pipe(f_p) == -1)) {
    perror("Père : erreur lors de la creation d'un tube ");
    exit(EXIT_FAILURE);
  }

  /* Création du fils */
  if((pid = fork()) == -1) {
    perror("Père : erreur lors de la creation du fils ");
    exit(EXIT_FAILURE);
  }
  if(pid == 0)
    fils(p_f, f_p);

  /* Fermeture des descripteurs inutiles */
  if((close(p_f[TUBE_LECTURE]) == -1) ||
     (close(f_p[TUBE_ECRITURE]) == -1)) {
    perror("Père : erreur lors de la fermeture des descripteurs ");
    exit(EXIT_FAILURE);
  }

  /* Ecriture et envoi de 5 entiers */
  tmp = 1;
  for(i = 0; i < 5; i++) {
    if(write(p_f[TUBE_ECRITURE], &tmp, sizeof(int)) == -1) {
      perror("Père : erreur lors de l'écriture d'un entier dans le tube ");
      exit(EXIT_FAILURE);
    }
    printf("Père : entier envoye : %d\n", tmp);
    if(read(f_p[TUBE_LECTURE], &tmp, sizeof(int)) == -1) {
      perror("Père : erreur lors de la lecture d'un entier dans le tube ");
      exit(EXIT_FAILURE);
    }
    printf("Père : entier lu : %d\n", tmp);
    tmp *= 2;
  }

  /* Fermeture des descripteurs */
  if((close(p_f[TUBE_ECRITURE]) == -1) ||
     (close(f_p[TUBE_LECTURE]) == -1)) {
    perror("Père : erreur lors de la fermeture des descripteurs ");
    exit(EXIT_FAILURE);
  }

  /* Attente de la fin du fils */
  if(wait(NULL) == -1) {
    perror("Père : erreur lors de l'attente de la fin du fils ");
    exit(EXIT_FAILURE);
  }
  printf("Père : fils terminé.\n");

  return EXIT_SUCCESS;
}
