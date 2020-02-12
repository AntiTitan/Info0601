/**
 * Ce programme permet de supprimer le tableau de sémaphores qui a ete crée par
 * le programme 'semaphoreServeur'.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/sem.h>    /* Pour semget, semctl */

int main() {
  int semid;

  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la recuperation du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  /* Suppression du tableau de sémaphores */
  if(semctl(semid, 0, IPC_RMID) == -1) {
    perror("Erreur lors de la suppresion du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  printf("Tableau de semaphores supprimé.\n");

  return EXIT_SUCCESS;
}
