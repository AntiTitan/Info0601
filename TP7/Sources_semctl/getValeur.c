/**
 * Ce programme permet d'illustrer le fonctionnement de la fonction semctl.
 * Il permet de récupérer la valeur d'un sémaphore ou de l'ensemble des
 * sémaphores d'un tableau de sémaphores.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, fprintf, perror */
#include <sys/sem.h>    /* Pour semget, semctl */

int main(int argc, char *argv[]) {
  int cle, semid, num = -1, i;
  struct semid_ds sem_buf;
  unsigned short *tableau, valeur;

  /* Vérification des arguments et récupération des valeurs */
  if((argc < 2) || (argc > 3)) {
    fprintf(stderr, "Usage: %s clé\n", argv[0]);
    fprintf(stderr, "\tAffiche toutes les valeurs d'un tableau de sémaphores.\n");
    fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
    fprintf(stderr, "Usage: %s clé num\n", argv[0]);
    fprintf(stderr, "\tAffiche la valeur d'un sémaphore d'un tableau de sémaphores.\n");
    fprintf(stderr, "\t'num' est le numéro du sémaphore.\n");
    fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
    exit(EXIT_FAILURE);
  }
  cle = atoi(argv[1]);
  if(argc == 3)
    num = atoi(argv[2]);

  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)cle, 0, 0)) == -1) {
    perror("Erreur lors de la récuperation du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  /* Récupération des informations sur le tableau de sémaphores */
  if(semctl(semid, 0, IPC_STAT, &sem_buf) == -1) {
    perror("Erreur lors de la récupération d'informations sur le tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  if(num == -1) {
    /* Allocation du tableau nécessaire */
    if((tableau = (unsigned short *)malloc(sizeof(unsigned short) * sem_buf.sem_nsems)) == NULL) {
      perror("Erreur lors de l'allocation mémoire ");
      exit(EXIT_FAILURE);
    }

    /* Récupération des valeurs du tableau de sémaphores */
    if(semctl(semid, 0, GETALL, tableau) == -1) {
      perror("Erreur lors de la récupération des valeurs du tableau de sémaphores ");
      exit(EXIT_FAILURE);
    }

    /* Affichage des valeurs */
    printf("Valeurs : [");
    for(i = 0; i < sem_buf.sem_nsems - 1; i++)
      printf("%d, ", tableau[i]);
    printf("%d]\n", tableau[sem_buf.sem_nsems - 1]);
    
    free(tableau);
  }
  else {
    /* Vérification du numéro de sémaphore spécifié */
    if((num < 0) || (num >= sem_buf.sem_nsems)) {
      fprintf(stderr, "Numero de sémaphore incorrect (%d), doit être dans [0; %ld]\n", num, sem_buf.sem_nsems - 1);
      exit(EXIT_FAILURE);
    }

    /* Récupération de la valeur du sémaphore */
    if((valeur = semctl(semid, num, GETVAL)) == -1) {
      perror("Erreur lors de la récupération de la valeur du sémaphore ");
      exit(EXIT_FAILURE);
    }
    
    printf("Valeur du sémaphore %d : %d\n", num, valeur);
  }

  return EXIT_SUCCESS;
}
