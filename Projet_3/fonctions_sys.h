#define _XOPEN_SOURCE
#ifndef FONCTIONS_SYS_H
#define FONCTIONS_SYS_H

#include <sys/types.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>      /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>       /* Pour printf, fprintf, perror */
#include <sys/socket.h>  /* Pour socket, bind */
#include <arpa/inet.h>   /* Pour sockaddr_in */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */
#include <signal.h>      /* Pour sigaction */
#include <pthread.h>
#include <sys/stat.h>    /* Pour S_IRUSR, S_IWUSR */

#include "struct_message.h"

int creerSemaphores(int CLE_S);
int supprimerSemaphores(int semid);
int recupererSemaphores(int CLE_S);
void Peux(int sem,int semid);
void Vas(int sem,int semid);

#endif