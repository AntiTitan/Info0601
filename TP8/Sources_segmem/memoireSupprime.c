/**
 * Ce programme permet de supprimer le segment de mémoire partagée crée par
 * le programme 'memoireServeur'.
 * @author Cyril Rabat
 **/
#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmctl */

int main() {
  int shmid;

  /* Récupération du segment de mémoire partagée */
  if((shmid = shmget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la recupération du segment de mémoire partagée ");
    exit(EXIT_FAILURE);
  }

  /* Suppression du segment de mémoire partagée */
  if(shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("Erreur lors de la suppression du segment de mémoire partagée ");
    exit(EXIT_FAILURE);
  }

  printf("Segment de memoire partagée supprimé.\n");

  return EXIT_SUCCESS;
}
