#include "f_voiture.h"

int recupFile(int CLE_F){
    int msqid;
    /* Récupération de la file */
    msqid = msgget((key_t)CLE_F, 0);
    return msqid;
}
int recupMemoire(int CLE_M){
    int shmid;
    shmid = shmget((key_t)CLE_M, 0, 0);
    return shmid;
}
int recupSemaphores(int CLE_S){
    int semid;
    semid = semget((key_t)CLE_S, 0, 0);
    return semid;
}
