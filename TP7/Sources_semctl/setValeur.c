/**
 * Ce programme permet d'illustrer le fonctionnement de la fonction semctl.
 * Il permet de spécifier la valeur d'un sémaphore ou de l'ensemble des
 * sémaphores d'un tableau de sémaphores.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, fprintf, perror */
#include <sys/sem.h>    /* Pour semget, semctl */
#include <string.h>     /* Pour strcmp */

/**
 * Affichage l'aide.
 * @param argv les paramètres du programme
 */
void afficheAide(char *argv[]) {
  fprintf(stderr, "Usage: %s cle ALL val1 val2 ... valn\n", argv[0]);
  fprintf(stderr, "\tModifie les valeurs des sémaphores d'un tableau de sémaphores.\n");
  fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
  fprintf(stderr, "\t'val1' ... 'valn' sont les valeurs des sémaphores.\n");
  fprintf(stderr, "Usage: %s clé SET num val\n", argv[0]);
  fprintf(stderr, "\tModifie la valeur d'un sémaphore d'un tableau de sémaphores.\n");
  fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
  fprintf(stderr, "\t'num' est le numéro du sémaphore.\n");
  fprintf(stderr, "\t'val' est le numéro du sémaphore.\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int cle, semid, num, valeur, i;
  struct semid_ds sem_buf;
  unsigned short *tableau;

  /* Vérification des arguments et récupération de la clé */
  if(argc < 4)
    afficheAide(argv);
  cle = atoi(argv[1]);

  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)cle, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  /* Récupération des informations sur le tableau de sémaphores */
  if(semctl(semid, 0, IPC_STAT, &sem_buf) == -1) {
    perror("Erreur lors de la récupération d'info sur le tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  if(strcmp(argv[2], "SET") == 0) {
    /* Récupération des arguments et vérifications */
    if(argc != 5)
      afficheAide(argv);
    num = atoi(argv[3]);
    valeur = atoi(argv[4]);
    if((num < 0) || (num >= sem_buf.sem_nsems)) {
      fprintf(stderr, "Numéro de sémaphore (%d) incorrect; doit être dans [0, %ld]\n", num, sem_buf.sem_nsems - 1);
      exit(EXIT_FAILURE);
    }

    /* Modification de la valeur */
    if(semctl(semid, num, SETVAL, valeur) == -1) {
      perror("Erreur lors de la modification de la valeur ");
      exit(EXIT_FAILURE);
    }

    printf("Valeur du sémaphore %d : %d\n", num, valeur);
  }
  else if(strcmp(argv[2], "ALL") == 0) {
    /* Vérification des arguments */
    if(argc != (3 + sem_buf.sem_nsems)) {
      fprintf(stderr, "Vous devez spécifier %ld valeurs.\n", sem_buf.sem_nsems);
      exit(EXIT_FAILURE);
    }

    /* Allocation du tableau nécessaire */
    if((tableau = (unsigned short *)malloc(sizeof(unsigned short) * sem_buf.sem_nsems)) == NULL) {
      perror("Erreur lors de l'allocation mémoire ");
      exit(EXIT_FAILURE);
    }

    /* Récupération des valeurs */
    for(i = 0; i < sem_buf.sem_nsems; i++)
      tableau[i] = atoi(argv[i + 3]);

    /* Modification des valeurs */
    if(semctl(semid, 0, SETALL, tableau) == -1) {
      perror("Erreur lors de la modification des valeurs ");
      exit(EXIT_FAILURE);
    }

    free(tableau);

    printf("Valeurs modifiées.\n");
  }
  else
    afficheAide(argv);

  return EXIT_SUCCESS;
}
