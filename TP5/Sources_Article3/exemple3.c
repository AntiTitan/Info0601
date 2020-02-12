/**
 * Ce programme montre l'utilisation de la fonction sigsuspend. Le programme
 * crée un processus fils qui se met en attente de réception de SIGUSR2.
 * Dès que c'est le cas, il s'arrête. C'est le père qui envoie le signal.
 * @author Cyril Rabat
 * @version 27/01/2020
 */
#define _POSIX_SOURCE

#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>    /* Pour sigXXX */
#include <unistd.h>    /* Pour fork */
#include <stdio.h>     /* Pour printf, perror */
#include <sys/types.h> /* Pour pid_t */
#include <sys/wait.h>  /* Pour wait */
#include <errno.h>     /* Pour errno */

void gestionnaire(int sig_num) {
  printf("J'ai reçu un SIGUSR2.\n");
}

void fils() {
  struct sigaction action;                        /* Pour positionner handler */
  sigset_t sigs_new;                              /* Signaux à bloquer */
  sigset_t sigs_wait;                             /* Signaux attendus */
  sigset_t sigs_old;                              /* Ancien masque */
  
  /* Blocage de tous les signaux */
  sigfillset(&sigs_new);
  if(sigprocmask(SIG_BLOCK, &sigs_new, &sigs_old) == -1) {
    perror("Erreur lors du blocage des signaux ");
    exit(EXIT_FAILURE);
  }

  /* Positionnement gestionnaire pour SIGUSR2 */
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  action.sa_handler = gestionnaire;
  if(sigaction(SIGUSR2, &action, NULL) == -1) {
    perror("Erreur lors du positionnement du gestionnaire ");
    exit(EXIT_FAILURE);
  }

  /* Remplissage des signaux attendus */
  sigfillset(&sigs_wait);
  sigdelset(&sigs_wait, SIGUSR2);

  /* Mise en attente de SIGUSR2 */
  printf("Fils : j'attends SIGUSR2.\n");
  if(sigsuspend(&sigs_wait) == -1) {
    if(errno != EINTR) {
      perror("Erreur lors de l'attente de SIGUSR2 ");
      exit(EXIT_FAILURE);
    }
  }
  printf("Fils : c'est bon, j'ai reçu SIGUSR2.\n");

  /* Replacer les gestionnaires par défaut */
  if(sigprocmask(SIG_SETMASK, &sigs_old, 0) == -1) {
    perror("Erreur lors du replacement des signaux ");
    exit(EXIT_FAILURE);
  }

  printf("Fils : j'ai terminé.\n");

  exit(EXIT_SUCCESS);
}

int main() {
  pid_t pid;

  /* Création du fils */
  if((pid = fork()) == -1) {
    perror("Erreur lors de la creation du fils ");
    exit(EXIT_FAILURE);
  }
  if(pid == 0)
    fils();
  printf("Père : fils lancé.\n");

  /* Envoi du signal SIGUSR1 */
  sleep(1);
  printf("Père : j'envoie SIGUSR1 au fils.\n");
  if(kill(pid, SIGUSR1) == -1) {
    perror("Erreur lors de l'envoi de SIGUSR1 ");
    exit(EXIT_FAILURE);
  }

  /* Envoi du signal SIGUSR2 */
  sleep(1);
  printf("Père : j'envoie SIGUSR2 au fils.\n");
  if(kill(pid, SIGUSR2) == -1) {
    perror("Erreur lors de l'envoi de SIGUSR2 ");
    exit(EXIT_FAILURE);
  }

  /* Attente de la fin du fils */
  sleep(1);
  if((wait(NULL)) == -1) {
    perror("Erreur lors de l'attente de mon fils ");
    exit(EXIT_FAILURE);
  }
  printf("Père : mon fils est arrêté.\n");

  return EXIT_SUCCESS;
}
