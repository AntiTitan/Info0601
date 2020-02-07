/*
Reception: seulement le type TYPE_TRIEUR
Envoie:
    TYPE_ENREGISTRER : une var de type enregistre_t
    TYPE_CLIENT : une var msgClient_t
*/

#define MAX_CLIENT 10
#define _XOPEN_SOURCE

#include <string.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

#include "struct.h"