/**
 * Ce programme vise a illustrer le fonctionnement des segments de mémoire
 * partagée IPC System V. Ce programme crée un segment de mémoire partagée et
 * y place 10 entiers.
 * @author Cyril Rabat
 **/
#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

int main() {
  int shmid, i;
  int *adresse;

  /* Création d'un segment de 10 entiers */
  if((shmid = shmget((key_t)CLE, sizeof(int) * 10, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST)
      fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", CLE);
    else
      perror("Erreur lors de la création du segment de mémoire ");
    exit(EXIT_FAILURE);
  }
  printf("Serveur : segment crée.\n");

  /* Attachement du segment de mémoire partagée */
  if((adresse = shmat(shmid, NULL, 0)) == (void*)-1) {
    perror("Erreur lors de l'attachement du segment de mémoire partagée ");
    exit(EXIT_FAILURE);
  }

  /* Placement des entiers dans le segment de mémoire partagée */
  for(i = 0; i < 10; i++)
    adresse[i] = i * 2;
  printf("Serveur : entiers placés dans le segment.\n");

  /* Détachement du segment de mémoire partagée */
  if(shmdt(adresse) == -1) {
    perror("Erreur lors du détachement ");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
