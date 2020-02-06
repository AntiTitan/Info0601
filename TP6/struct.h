#ifndef _STRUCT_
#define _STRUCT_

/* Taille max de la chaine de char */
#define CHAINE_MAX 20

/* Type de message enregistrer ou arreter un client */
#define TYPE_ENREGISTRE 100
/* Type de message envoye par le client au trieur */
#define TYPE_CLIENT 200
/* Type de message envoye par le trieur au clilent */
#define TYPE_TRIEUR 300

/* Structure utilisee pour les requetes/ par le trieur pour envoyer un msg au client */
typedef struct {
    long type;
    char chaine [CHAINE_MAX];
} requete_t;

/* Structure utilisee pour s'enregistrer ou s'arreter */
typedef struct {
    long type;
    int id;
} enregistre_t;

/* Structure utilisee par le client pour envoyer un msg */
typedef struct {
    long type;
    int id;
    int dest;
    char chaine [CHAINE_MAX];
} msgClient_t;

#endif
