#ifndef STRUCT_H
#define STRUCT_H

#include "ncurses.h"
#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <unistd.h> 

#define TYPE_RECUP_CONFIG 1
#define TYPE_ENVOI_CONFIG 2
#define TYPE_MODIF_CARTE  3

typedef struct {
    long type;
    pid_t pid; /*pid demandeur des clé du seg memoire et tableau séaphore*/
} r_config_t;

typedef struct {
    long type;
    key_t cle_mem;
    key_t cle_sema;
} e_config_t;

typedef struct {
    long type;
    /*un message ?*/
    int voiture;
} modif_carte_t;


#endif