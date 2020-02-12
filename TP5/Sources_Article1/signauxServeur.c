/**
 * Ce programme illustre le fonctionnement de l'appel système sigaction qui
 * permet de spécifier un gestionnaire à des signaux. Le programme attend
 * de reçevoir un signal SIGUSR1 puis un signal SIGUSR2. Pour cela, il faut
 * utiliser le programme signauxClient.
 * @author Cyril Rabat
 * @version 27/01/2020
 */
#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>    /* Pour sigaction */
#include <stdio.h>     /* Pour printf */
#include <unistd.h>     /* Pour pause */

int cpt = 0;    /* Permet de marquer les signaux recus */

/**
 * Ceci est le gestionnaire qui est associé aux signaux SIGUSR1 et SIGUSR2.
 * @param signum le numéro du signal reçu
 */
void handler(int signum) {
  if(signum == SIGUSR1) {
    printf("Signal 1 reçu\n");
    cpt = cpt | 1;
  }
  if(signum == SIGUSR2) {
    printf("Signal 2 reçu\n");
    cpt = cpt | 2;
  }
}

int main() {
  struct sigaction action;

  /* Positionnement du gestionnaire pour SIGUSR1 */
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  if(sigaction(SIGUSR1, &action, NULL) == -1) {
    perror("Erreur lors du positionnement ");
    exit(EXIT_FAILURE);
  }

  /* Positionnement du gestionnaire pour SIGUSR2 */
  if(sigaction(SIGUSR2, &action, NULL) == -1) {
    perror("Erreur lors du positionnement ");
    exit(EXIT_FAILURE);
  }

  printf("Prêt à recevoir des signaux. Mon PID : %d\n", getpid());

  /* Mise en attente jusqu'à recevoir au moins un signal SIGUSR1 et SIGUSR2 */
  while(cpt != 3) {
    pause();
  }

  return EXIT_SUCCESS;
}
