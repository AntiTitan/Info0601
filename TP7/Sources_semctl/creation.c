/**
 * Ce programme permet de créer un tableau de sémaphore. La clé et le nombre
 * de sémaphores sont passés en paramètres.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/sem.h>    /* Pour semget */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

int main(int argc, char *argv[]) {
  int cle, semid, nbSem;

  /* Vérification des arguments et récupération des valeurs */
  if(argc != 3) {
    fprintf(stderr, "Usage: %s clé nbSem\n", argv[0]);
    fprintf(stderr, "\tCrée un nouveau tableau de sémaphores.\n");
    fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
    fprintf(stderr, "\t'nbSem' est le nombre de sémaphores dans le tableau.\n");
    exit(EXIT_FAILURE);
  }
  cle = atoi(argv[1]);
  nbSem = atoi(argv[2]);
  
  /* Création du tableau de sémaphores */
  if((semid = semget((key_t)cle, nbSem, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST)
      fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", cle);
    else
      perror("Erreur lors de la création du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  printf("Tableau de %d sémaphores crée, clé = %d.\n", nbSem, cle);

  return EXIT_SUCCESS;
}
