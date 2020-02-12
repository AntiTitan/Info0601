/**
 * Ce programme montre l'utilisation des ensembles de signaux, ainsi que les
 * fonctions associées et la fonction sigprocmask. Le programme crée un
 * processus fils qui ignore tous les signaux sauf SIGINT (et les autres
 * signaux non blocables). Le fils se met ensuite en pause. Le pere envoie
 * d'abord un signal SIGUSR1 qui est bien ignoré, puis un signal SIGINT qui
 * lui n'est pas bloqué. Le fils termine alors son exécution et le père aussi.
 * @author Cyril Rabat
 * @version 27/01/2020
 */
#define _POSIX_SOURCE

#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>    /* Pour sigXXX */
#include <unistd.h>    /* Pour fork */
#include <stdio.h>     /* Pour printf, perror */
#include <sys/types.h> /* Pour pid_t */
#include <sys/wait.h>  /* Pour waitpid */

void fils() {
  sigset_t sigs_new;                              /* Signaux à bloquer */
  sigset_t sigs_old;                              /* Ancien masque */

  /* Blocage de tous les signaux sauf SIGINT */
  sigfillset(&sigs_new);
  sigdelset(&sigs_new, SIGINT);
  if(sigprocmask(SIG_BLOCK, &sigs_new, &sigs_old) == -1) {
    perror("Erreur lors du blocage des signaux ");
    exit(EXIT_FAILURE);
  }

  /* Mise en attente de signaux */
  printf("Fils : je suis insensible à tous les signaux !\n");
  pause();
  printf("Fils : je suis sorti de ma pause\n");

  /* Replacer par défaut : inutile ! */
  if(sigprocmask(SIG_SETMASK, &sigs_old, 0)) {
    perror("Erreur lors du replacement des signaux bloqués ");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

int main() {
  pid_t pid, tmp;

  /* Création du fils */
  if((pid = fork()) == -1) {
    perror("Erreur lors de la création du premier fils ");
    exit(EXIT_FAILURE);
  }
  if(pid == 0)
    fils();

  /* Envoi du signal SIGUSR1 */
  printf("Père : fils lancé.\n");
  sleep(1);
  printf("Père : j'envoie SIGUSR1 au fils.\n");
  if(kill(pid, SIGUSR1) == -1) {
    perror("Erreur lors de l'envoi de SIGUSR1 ");
    exit(EXIT_FAILURE);
  }

  /* Vérification de la présence du fils */
  if((tmp = waitpid(pid, NULL, WNOHANG)) == -1) {
    perror("Erreur lors de l'attente de mon fils ");
    exit(EXIT_FAILURE);
  }
  if(tmp != 0)
    printf("Père : bon, je crois que j'ai tué mon fils...\n");
  else
    printf("Père : mon fils est toujours en vie...\n");
  
  /* Envoi du signal SIGINT */
  printf("Père : j'envoie maintenant SIGINT.\n");
  if(kill(pid, SIGINT) == -1) {
    perror("Erreur lors de l'envoi de SIGINT ");
    exit(EXIT_FAILURE);
  }

  /* Vérification de la présence du fils */
  sleep(1);
  if((tmp = waitpid(pid, NULL, WNOHANG)) == -1) {
    perror("Erreur lors de l'attente de mon fils ");
    exit(EXIT_FAILURE);
  }
  if(tmp != 0)
    printf("Père : bon, je crois que j'ai tué mon fils...\n");
  else
    printf("Père : mon fils est toujours en vie...\n");

  return EXIT_SUCCESS;
}
