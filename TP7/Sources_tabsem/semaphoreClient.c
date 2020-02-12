/**
 * Ce programme vise a illustrer le fonctionnement des tableaux de sémaphores.
 * Les deux programmes exécutent les blocs de code suivants :
 *     semaphoreServeur   A    B    C
 *     semaphoreClient    D    E    F
 * Les relations de dependance : A -> E et E -> C
 * La première est résolue avec le sémaphore S0 et la seconde par S1.
 * @author Cyril Rabat
 **/
#include <stdio.h>    /* Pour printf, perror */
#include <stdlib.h>   /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/sem.h>  /* Pour semget, semop */
#include <errno.h>    /* Pour errno */

int main() {
  int semid;
  struct sembuf op;

  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  printf("Client : bloc D exécuté\n");

  /* Réalisation de P(S0) */
  printf("Client : attente du sémaphore S0 -> P(S0)\n");
  op.sem_num = 0;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  printf("Client : bloc E exécuté\n");

  /* Réalisation de V(S1) */
  printf("Client : libération du sémaphore S1 -> V(S1)\n");
  op.sem_num = 1;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  printf("Client : bloc F exécuté\n");

  return EXIT_SUCCESS;
}
