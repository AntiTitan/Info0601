/**
 * Ce programme montre l'utilisation du blocage de signaux à l'aide de
 * sigprocmask, la vérification de signaux reçus à l'aide de sigpending.
 * Le processus fils bloque tous les signaux et se met en pause de 5s.
 * Une fois sorti de sa pause, il vérifie s'il a reçu des signaux.
 * Le père se contente d'envoyer les signaux au fils et d'attendre sa fin.
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

void fils() {
  sigset_t sigs_new;                              /* Signaux a bloquer */
  sigset_t sigs_old;                              /* Ancien masque */
  sigset_t sigs_bloques;                          /* Signaux bloques */
  
  /* Blocage de tous les signaux */
  sigfillset(&sigs_new);
  if(sigprocmask(SIG_BLOCK, &sigs_new, &sigs_old) == -1) {
    perror("Erreur lors du blocage des signaux ");
    exit(EXIT_FAILURE);
  }

  /* Mise en attente de signaux */
  printf("Fils : je suis insensible à tous les signaux !\n");
  sleep(5);
  printf("Fils : je suis sorti de ma pause\n");

  /* Récupération des signaux bloqués */
  if(sigpending(&sigs_bloques) == -1) {
    perror("Erreur lors de la récupération des signaux bloqués ");
    exit(EXIT_FAILURE);
  }

  /* Vérification des signaux bloqués */
  if(sigismember(&sigs_bloques, SIGUSR1))
    printf("Fils : j'ai bien reçu SIGUSR1 pendant ma pause.\n");
  else
    printf("Fils : je n'ai pas reçu SIGUSR1 pendant ma pause.\n");
  if(sigismember(&sigs_bloques, SIGUSR2))
    printf("Fils : j'ai bien reçu SIGUSR2 pendant ma pause.\n");
  else
    printf("Fils : je n'ai pas reçu SIGUSR2 pendant ma pause.\n");

  /* Replacer par défaut : inutile ! */
  if(sigprocmask(SIG_SETMASK, &sigs_old, 0)) {
    perror("Erreur lors du replacement des signaux bloqués ");
    exit(EXIT_FAILURE);
  }

  printf("Fils : j'ai terminé.\n");

  exit(EXIT_SUCCESS);
}

int main() {
  pid_t pid;

  /* Création du fils */
  if((pid = fork()) == -1) {
    perror("Erreur lors de la création du premier fils ");
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
