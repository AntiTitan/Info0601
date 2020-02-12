/**
 * Ce programme vise à illustrer le fonctionnement des segments de mémoire
 * partagée IPC System V. Ce programme ouvre un segment de mémoire partagée qui
 * est crée par 'memoireServeur' et y lit les 10 entiers présents.
 * @author Cyril Rabat
 **/
#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */

int main() {
  int shmid, i;
  int *adresse;

  /* Récupération du segment de mémoire partagée */
  if((shmid = shmget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du segment de mémoire partagée ");
    exit(EXIT_FAILURE);
  }
  printf("Client : récupération du segment de mémoire partagée.\n");

  /* Attachement du segment de mémoire partagée */
  if((adresse = shmat(shmid, NULL, 0)) == (void*)-1) {
    perror("Erreur lors de l'attachement du segment de mémoire partagée ");
    exit(EXIT_FAILURE);
  }

  /* Lecture des 10 entiers du segment de mémoire partagée */
  printf("Client : entiers lus = [");
  for(i = 0; i < 10; i++) {
    printf("%d", adresse[i]);
    if(i < 9) printf(", ");
  }
  printf("]\n");

  /* Détachement du segment de mémoire partagée */
  if(shmdt(adresse) == -1) {
    perror("Erreur lors du détachement du segment de mémoire partagée ");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
