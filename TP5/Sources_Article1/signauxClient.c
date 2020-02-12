/**
 * Ce programme montre comment envoyer des signaux à l'aide de l'appel
 * systeme kill. Le PID du programme à qui envoyer les signaux est specifié
 * en argument (ici, le but est d'exécuter le programme signauxServeur et
 * de donner son PID au programme signauxClient). Un signal SIGUSR1 est
 * envoyé, suivi de SIGUSR2.
 * @author Cyril Rabat
 * @version 27/01/2020
 */
#define _POSIX_SOURCE

#include <signal.h>    /* Pour kill */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>     /* Pour printf */
#include <unistd.h>    /* Pour sleep */
#include <sys/types.h> /* Pour pid_t */

int main(int argc, char *argv[]) {
  pid_t pidServeur;

  /* Recuperation des arguments */
  if(argc != 2) {
    fprintf(stderr, "Tapez %s pid\n", argv[0]);
    fprintf(stderr, "Où\n\tpid : pid du serveur\n");
    exit(EXIT_FAILURE);
  }
  pidServeur = atoi(argv[1]);

  /* Envoi du premier signal */
  printf("Attente avant envoi premier signal\n");
  sleep(1);
  printf("Envoi premier signal\n");
  if(kill(pidServeur, SIGUSR1) == -1) {
    perror("Erreur lors de l'envoi du signal ");
    exit(EXIT_FAILURE);
  }

  /* Envoi du deuxieme signal */
  printf("Attente avant envoi deuxieme signal\n");
  sleep(2);
  printf("Envoi deuxieme signal\n");
  if(kill(pidServeur, SIGUSR2) == -1) {
    perror("Erreur lors de l'envoi du signal ");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
