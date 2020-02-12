/**
 * Ce programme vise a illustrer le fonctionnement des tableaux de sémaphores.
 * Les deux programmes exécutent les blocs de code suivants :
 *     semaphoreServeur   A    B    C
 *     semaphoreClient    D    E    F
 * Les relations de dépendance : A -> E et E -> C
 * La première est résolue avec le sémaphore S0 et la seconde par S1.
 * @author Cyril Rabat
 **/
#include <stdio.h>    /* Pour printf, perror */
#include <stdlib.h>   /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/sem.h>  /* Pour semget, semctl, semop */
#include <errno.h>    /* Pour errno */
#include <sys/stat.h> /* Pour S_IRUSR, S_IWUSR */

int main() {
  int semid;
  unsigned short val[2] = {0, 0};
  struct sembuf op;

  /* Création du tableau de sémaphore */
  if((semid = semget((key_t)CLE, 2, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST)
      fprintf(stderr, "Tableau de sémaphores (cle=%d) existant\n", CLE);
    else
      perror("Erreur lors de la création du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  /* Initialisation des sémaphores */
  if(semctl(semid, 0, SETALL, val) == -1) {
    perror("Erreur lors de l'initialisation des sémaphores ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur : bloc A exécuté\n");
  
  /* Réalisation de V(S0) */
  printf("Serveur : libération du semaphore S0 -> V(S0)\n");
  op.sem_num = 0;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur : bloc B exécuté\n");

  /* Réalisation de P(S1) */
  printf("Serveur : attente du sémaphore S1 -> P(S1)\n");
  op.sem_num = 1;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  printf("Serveur : bloc C exécuté\n");

  return EXIT_SUCCESS;
}
