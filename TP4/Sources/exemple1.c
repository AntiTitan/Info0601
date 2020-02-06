/**
 * Ce programme montre comment créer un tube anonyme entre un processus père
 * et un processus fils. Il est ensuite utilisé pour envoyer 5 entiers du
 * père vers le fils.
 * @author Cyril Rabat
 * @version 21/01/2020
 */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf, perror */
#include <unistd.h>    /* Pour fork, pipe */
#include <sys/wait.h>  /* Pour wait */
#include <sys/types.h> /* Pour pid_t */

void fils(int tube[2]) {
  int i, tmp;

  /* Fermeture du tube en écriture */
  if(close(tube[1]) == -1) {
    perror("Fils : erreur lors de la fermeture du tube en écriture ");
    exit(EXIT_FAILURE);
  }

  /* Lecture de 5 entiers */
  for(i = 0; i < 5; i++) {
    if(read(tube[0], &tmp, sizeof(int)) == -1) {
      perror("Fils : erreur lors de la lecture d'un entier depuis le tube ");
      exit(EXIT_FAILURE);
    }
    printf("Fils : entier lu : %d\n", tmp);
  }

  /* Fermeture du tube en lecture */
  if(close(tube[0]) == -1) {
    perror("Fils : erreur lors de la fermeture du tube en lecture ");
    exit(EXIT_FAILURE);
  }

  printf("Fils : terminé.\n");
  
  exit(EXIT_SUCCESS);
}

int main() {
  pid_t pid;
  int tube[2], i;

  /* Création du tube */
  if(pipe(tube) == -1) {
    perror("Père : erreur lors de la création du tube ");
    exit(EXIT_FAILURE);
  }

  /* Création du fils */
  if((pid = fork()) == -1) {
    perror("Père : erreur lors de la création du fils ");
    exit(EXIT_FAILURE);
  }
  if(pid == 0)
    fils(tube);

  /* Fermeture du tube en lecture */
  if(close(tube[0]) == -1) {
    perror("Père : erreur lors de la fermeture du tube en lecture ");
    exit(EXIT_FAILURE);
  }

  /* Envoi de 5 entiers */
  for(i = 0; i < 5; i++) {
    if(write(tube[1], &i, sizeof(int)) == -1) {
      perror("Père : erreur lors de l'écriture d'un entier dans le tube ");
      exit(EXIT_FAILURE);
    }
    printf("Père : entier envoyé : %d\n", i);
    sleep(1);
  }

  /* Fermeture du tube en écriture */
  if(close(tube[1]) == -1) {
    perror("Père : erreur lors de la fermeture du tube en ecriture ");
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
