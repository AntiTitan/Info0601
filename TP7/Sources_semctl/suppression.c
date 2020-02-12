/**
 * Ce programme permet de supprimer un tableau de sémaphore dont la clé est
 * passée en argument.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, fprintf, perror */
#include <sys/sem.h>    /* Pour semget, semctl */

int main(int argc, char *argv[]) {
  int cle;
  int semid;

  /* Vérification des arguments et récupération de la clé */
  if(argc != 2) {
    fprintf(stderr, "Usage: %s clé\n", argv[0]);
    fprintf(stderr, "\tSupprime le tableau de sémaphores specifié.\n");
    fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
    exit(EXIT_FAILURE);
  }
  cle = atoi(argv[1]);
  
  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)cle, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  /* Suppression du tableau de sémaphores */
  if(semctl(semid, 0, IPC_RMID) == -1) {
    perror("Erreur lors de la suppresion du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  printf("Tableau de sémaphores (clé=%d) supprimé.\n", cle);

  return EXIT_SUCCESS;
}
