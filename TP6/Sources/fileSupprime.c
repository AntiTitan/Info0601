/**
 * Ce programme permet de supprimer la file de messages qui a ete creee par
 * le programme 'fileServeur'.
 * @author Cyril Rabat
 **/
#define _XOPEN_SOURCE

#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgctl */
#include <errno.h>      /* Pour errno */

#include "structures.h"

int main() {
  int msqid;

  /* Recuperation de la file */
  if((msqid = msgget((key_t)CLE, 0)) == -1) {
    if(errno == ENOENT)
      fprintf(stderr, "Aucune file présente avec la clé %d.\n", (key_t)CLE);
    else
      perror("Erreur lors de la récupération de la file ");
    exit(EXIT_FAILURE);
  }

  /* Suppression de la file */
  if(msgctl(msqid, IPC_RMID, 0) == -1) {
    perror("Erreur lors de la suppression de la file ");
    exit(EXIT_FAILURE);
  }

  printf("File supprimée.\n");

  return EXIT_SUCCESS;
}
