#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>  /* Pour semget, semctl, semop */
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> /* Pour S_IRUSR, S_IWUSR */
#include <errno.h>    /* Pour errno */
#include <sys/wait.h>
#include <sys/types.h>

#include "include.h"
#define NBSEM 3

/**
 * Génère un entier aléatoire dans un intervalle [a, b].
 * @param a la borne minimale
 * @param b la borne maximale
 * @return l'entier aléatoire
 */
int alea(int a, int b) {
  return rand() % (b - a + 1) + a;
}

/**
 * Écriture d'un message vers le père.
 * @param fd le descripteur du tube en écriture vers le père.
 * @param num le numéro du bloc
 * @param action l'action (début ou fin)
 */
void ecriture(int fd, int num, int action) {
  message_t msg;
  
  msg.num = num;
  msg.action = action;
  if(write(fd, &msg, sizeof(msg)) == -1) {
    fprintf(stderr, "Erreur lors de l'écriture vers le père");
    perror(" ");
    exit(EXIT_FAILURE);
  }
}

/**
 * Routine du fils 1.
 * @param fd le descripteur du tube en écriture vers le père.
 */
int fils1(int fd) {
  int semid;
  struct sembuf op;
  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL) + getpid());

  usleep(alea(MIN, MAX));

  /* Réalisation de P(S2) */
  printf("Fils1 : attente du sémaphore S2 -> P(S2)\n");
  op.sem_num = 2;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  /* Bloc A1 */
  ecriture(fd, A1, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, A1, FIN);
  /* Fin bloc A1 */

  /* Réalisation de V(S2) */
  printf("Fils1 : libération du semaphore S2 -> V(S2)\n");
  op.sem_num = 2;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  usleep(alea(MIN, MAX));

  /* Bloc B1 */
  ecriture(fd, B1, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, B1, FIN);
  /* Fin bloc B1 */
  
  usleep(alea(MIN, MAX));

  /* Réalisation de P(S1) */
  printf("Fils1 : attente du sémaphore S1 -> P(S1)\n");
  op.sem_num = 1;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  /* Bloc C1 */
  ecriture(fd, C1, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, C1, FIN);
  /* Fin bloc C1 */

  exit(EXIT_SUCCESS);
}

/**
 * Routine du fils 2.
 * @param fd le descripteur du tube en écriture vers le père. 
 */
int fils2(int fd) {
  int semid;
  struct sembuf op;

  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL) + getpid());

  usleep(alea(MIN, MAX));

  /* Bloc A2 */
  ecriture(fd, A2, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, A2, FIN);
  /* Fin bloc A2 */

  usleep(alea(MIN, MAX));

  /* Réalisation de P(S2) */
  printf("Fils2 : attente du sémaphore S2 -> P(S2)\n");
  op.sem_num = 2;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  /* Bloc B2 */
  ecriture(fd, B2, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, B2, FIN);
  /* Fin bloc B2 */

  /* Réalisation de V(S2) */
  printf("Fils2 : libération du semaphore S2 -> V(S2)\n");
  op.sem_num = 2;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  /* Réalisation de V(S0) */
  printf("Fils2 : libération du semaphore S0 -> V(S0)\n");
  op.sem_num = 0;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  usleep(alea(MIN, MAX));

  /* Bloc C2 */
  ecriture(fd, C2, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, C2, FIN);
  /* Fin bloc C2 */

  exit(EXIT_SUCCESS);
}

/**
 * Routine du fils 3.
 * @param fd le descripteur du tube en écriture vers le père.
 */
int fils3(int fd) {
  int semid;
  struct sembuf op;

  /* Récupération du tableau de sémaphores */
  if((semid = semget((key_t)CLE, 0, 0)) == -1) {
    perror("Erreur lors de la récupération du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL) + getpid());

  usleep(alea(MIN, MAX));

  /* Réalisation de P(S2) */
  printf("Fils3 : attente du sémaphore S2 -> P(S2)\n");
  op.sem_num = 2;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  /* Bloc A3 */
  ecriture(fd, A3, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, A3, FIN);
  /* Fin bloc A3 */

  /* Réalisation de V(S2) */
  printf("Fils3 : libération du semaphore S2 -> V(S2)\n");
  op.sem_num = 2;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  usleep(alea(MIN, MAX));

  /* Bloc B3 */
  ecriture(fd, B3, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, B3, FIN);
  /* Fin bloc B3 */

  usleep(alea(MIN, MAX));

  /* Réalisation de P(S0) */
  printf("Fils3 : attente du sémaphore S0 -> P(S0)\n");
  op.sem_num = 0;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  /* Bloc C3 */
  ecriture(fd, C3, DEBUT);
  usleep(alea(MIN, MAX));
  ecriture(fd, C3, FIN);
  /* Fin bloc C3 */

  /* Réalisation de V(S1) */
  printf("Fils3 : libération du semaphore S1 -> V(S1)\n");
  op.sem_num = 1;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(semid, &op, 1) == -1) {
    perror("Erreur lors de l'opération sur le sémaphore ");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

/**
 * Fonction principale.
 * @param argc le nombre d'arguments
 * @param argv les arguments
 * @return le code de retour
 */
int main(int argc, char *argv[]) {
  unsigned short val[3] = {0, 0, 1};
  int i, n, erreur, erreurs = 0;
  pid_t tube[2];
  char *blocs[9] = { "A1", "B1", "C1", "A2", "B2", "C2", "A3", "B3", "C3" };
  int etats[9];
  int semid;
  message_t msg;

  /* Création du tableau de sémaphores */
  if((semid = semget((key_t)CLE, NBSEM, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
    if(errno == EEXIST)
      fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE);
    else
      perror("Erreur lors de la création du tableau de sémaphores ");
    exit(EXIT_FAILURE);
  }
  
  /*S0(B2, C3), S1(C1, C3), S2(A1, A3, B2)*/
  /* Initialisation des sémaphores */
  if(semctl(semid, 0, SETALL, val) == -1) {
    perror("Erreur lors de l'initialisation des sémaphores ");
    exit(EXIT_FAILURE);
  }

  /* Initialisation des états */
  for(i = 0; i < 9; i++)
    etats[i] = UNDEF;

  /* Création du tube */
  if(pipe(tube) == -1) {
    perror("Erreur lors de la création du tube ");
    exit(EXIT_FAILURE);
  }

  /* Création des fils */
  for(i = 1; i <= 3; i++) {
    n = fork();
    if(n == -1) {
      fprintf(stderr, "Erreur lors de la creation du fils %d", i);
      perror(" ");
      exit(EXIT_FAILURE);
    }
    else if(n == 0) {
      /* Fermeture du descripteur inutile */
      if(close(tube[LECTURE]) == -1) {
        fprintf(stderr, "Erreur lors de la fermeture du tube en lecture dans le fils %d\n", i);
        exit(EXIT_FAILURE);
      }
      
      /* Lancement des routines des fils */      
      switch(i) {
        case 1: fils1(tube[ECRITURE]); break;
        case 2: fils2(tube[ECRITURE]); break;
        case 3: fils3(tube[ECRITURE]); break;
      }
    }
  }
  
  /* Fermeture du tube en écriture */
  if(close(tube[ECRITURE]) == -1) {
    fprintf(stderr, "Erreur lors de la fermeture du tube en lecture dans le père\n");
    exit(EXIT_FAILURE);
  }
  
  /* Lecture des actions */
  for(i = 0; i < 18; i++) {
    /* Lecture de l'action */
    if(read(tube[LECTURE], &msg, sizeof(msg)) == -1) {
      perror("Erreur lors de la lecture du message dans le père ");
      exit(EXIT_FAILURE);
    }
    
    /* Affichage de l'action */
    if(msg.action == DEBUT) {
      /* Vérification des actions */
      erreur = 0;
      switch(msg.num) {
      case A1:
        erreur = (etats[B2] == DEBUT) || (etats[A3] == DEBUT);
        break;
      case A3:
        erreur = (etats[A1] == DEBUT) || (etats[B2] == DEBUT);
        break;
      case B2:
        erreur = (etats[A1] == DEBUT) || (etats[A3] == DEBUT);
        break;
      case C1:
        erreur = (etats[B2] != FIN) || (etats[C3] != FIN);
        break;
      case C3:
        erreur = (etats[B2] != FIN);
        break;
      }

      /* Affichage de l'état */
      printf("Début du bloc %s ", blocs[msg.num]);
      if(erreur) {
        erreurs++;
        printf("\033[0;31m[ERREUR]\033[0m\n");
      }
      else
        printf("\033[0;32m[OK]\033[0m\n");
    }
    else
      printf("Fin du bloc %s\n", blocs[msg.num]);
    etats[msg.num] = msg.action;
  }

  /* Attente de la fin des fils */
  for(i = 0; i < 3; i++) {
    if(wait(NULL) == -1) {
      perror("Erreur lors de la fin d'un fils ");
      exit(EXIT_FAILURE);
    }
  }

  /* Message de fin */
  if(erreurs != 0)
    printf("L'exécution a échouée.\n");
  else
    printf("L'exécution a réussie.\n");

  return EXIT_SUCCESS;
}
