#ifndef _STRUCT_
#define _STRUCT_

/* Taille max de la chaine de char */
#define CHAINE_MAX 20

/* Structure utilisee pour les requetes/ par le trieur pour envoyer un msg au client */
typedef struct {
    pid_t pid;
    char chaine [CHAINE_MAX];
} msg_t;


#endif