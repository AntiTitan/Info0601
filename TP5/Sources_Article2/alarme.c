/**
 * Le but de ce programme est de montrer le fonctionnement de l'appel
 * système alarm. Une alarme de 5s est mise en place et le programme se
 * met en attente d'un signal quelconque. Dès que le signal SIGARLM est
 * reçu, le programme peut se terminer.
 * @author Cyril Rabat
 * @version 27/01/2020
 */
#define _POSIX_C_SOURCE 199309L
 
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf */
#include <unistd.h>    /* Pour pause */
#include <signal.h>    /* Pour signal */

int stop = 0;  /* Cette variable passe à 1 pour stopper le programme */

void handler(int signum) {
  stop = 1;
  if(signum == SIGALRM)
    printf("Signal SIGALRM reçu.\n");
  else {
    alarm(0);
    printf("Signal d'arrêt reçu.\n");
  }
}

int main() {
  struct sigaction action;

  /* Positionnement du gestionnaire pour SIGALRM et SIGINT */
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  if(sigaction(SIGALRM, &action, NULL) == -1) {
      perror("Erreur lors du positionnement ");
    exit(EXIT_FAILURE);
  }
  if(sigaction(SIGINT, &action, NULL) == -1) {
      perror("Erreur lors du positionnement ");
    exit(EXIT_FAILURE);
  }

  /* Armement d'une alarme */
  printf("Je place une alarme de 5s.\n");
  alarm(5);

  /* Mise en pause */
  printf("J'attends la fin de l'alarme ou un SIGINT...\n");
  while(stop == 0) {
    pause();
  }

  printf("Au revoir...\n");

  return EXIT_SUCCESS;
}
