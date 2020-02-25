#ifndef STRUCT_H
#define STRUCT_H

#define _XOPEN_SOURCE
#include "ncurses.h"
#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h> 
#include <signal.h>    /* Pour sigaction */

#define TYPE_RECUP_CONFIG 1
#define TYPE_ENVOI_CONFIG 2
#define TYPE_MODIF_CARTE  3
#define MAX_VOITURE       10

typedef struct rconfig {
    long type;
    pid_t pid; /*pid demandeur des clé du seg memoire et tableau séaphore*/
} r_config_t;

typedef struct econfig {
    long type;
    key_t cle_mem;
    key_t cle_sema;
} e_config_t;

typedef struct mcarte {
    long type;
    /*un message ?*/
    int voiture;
} modif_carte_t;


#endif