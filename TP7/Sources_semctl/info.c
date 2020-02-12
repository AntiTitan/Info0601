/**
 * Ce programme permet d'illustrer le fonctionnement de la fonction semctl.
 * Il affiche les heures de la dernière modification et de la dernière
 * opération, ainsi que le nombre de sémaphores du tableau de sémaphores.
 * @author Cyril Rabat
 **/
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, fprintf, perror */
#include <sys/sem.h>    /* Pour semget, semctl */
#include <time.h>       /* Pour ctime */

int main(int argc, char *argv[]) {
  int cle;
  int semid;
  struct semid_ds sem_buf;

  /* Vérification des arguments et récupération de la clé */
  if(argc != 2) {
    fprintf(stderr, "Usage: %s clé\n", argv[0]);
    fprintf(stderr, "\tAffiche les informations sur le tableau de sémaphores.\n");
    fprintf(stderr, "\t'clé' est la clé IPC du tableau de sémaphores.\n");
    exit(EXIT_FAILURE);
  }
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

  /* Affichage des informations */
  if(sem_buf.sem_otime == 0)
    printf("Date dernière opération    : encore aucune opération\n");
  else
    printf("Date dernière opération    : %s", ctime(&sem_buf.sem_otime));
  printf("Date dernière modification : %s", ctime(&sem_buf.sem_ctime));
  printf("Nombre de sémaphores       : %ld\n", sem_buf.sem_nsems);
  
  return EXIT_SUCCESS;
}
