#ifndef _INCLUDE
#define _INCLUDE

/* Constantes pour les pauses */
#define MIN 100
#define MAX 1000

/* Constantes pour les tubes */
#define LECTURE 0
#define ECRITURE 1

/* L'état d'un bloc */
#define UNDEF -1
#define DEBUT 0
#define FIN 1

/* Les blocs */
#define A1 0
#define B1 1
#define C1 2
#define A2 3
#define B2 4
#define C2 5
#define A3 6
#define B3 7
#define C3 8

/* La structure pour les messages */
typedef struct {
  int num;
  int action;
} message_t;

#endif
