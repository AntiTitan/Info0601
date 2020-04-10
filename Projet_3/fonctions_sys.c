#define _XOPEN_SOURCE
#include "fonctions_sys.h"


int creerSemaphores(int CLE_S) {
    int semid;

    if((semid = semget((key_t)CLE_S, MAX_PARTIE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le tableau de sémaphores (cle=%d) existe déjà.\n", CLE_S);
        else
            fprintf(stderr, "Erreur lors de la création du tableau de sémaphores \n");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int supprimerSemaphores(int semid) {
    if(semctl(semid, 0, IPC_RMID) == -1) {
        fprintf(stderr, "Erreur lors de la suppresion du tableau de sémaphores Controleur \n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdin, "Tableau de sémaphores supprimé.\n");
    return EXIT_SUCCESS;
}

int recupererSemaphores(int CLE_S) {
    int semid;
    if((semid = semget((key_t)CLE_S, 0, 0)) == -1) {
        fprintf(stderr, "Erreur lors de la récupération du tableau de sémaphoresss \n");
        exit(EXIT_FAILURE);
    }
    return semid;
}

void videMessage(message_t* msg){
    msg->typeMessage=-1;
    msg->position[0]=-1;
    msg->position[1]=-1;
    msg->idJoueur=-1;
    msg->idPartie=-1;
}

/**
 * P(S) avec S un sémaphore.
 * @param sem     le sémaphore concerne
 * @param semid   le tableau de semaphores
 * @return void
 */
void Peux(int sem, int semid) {
    struct sembuf op;

    /* Réalisation de P(Sxx) */
    op.sem_num = sem;
    op.sem_op = -1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore Peux(%d)\n", sem);
        switch(errno){
            case E2BIG : fprintf(stderr, "E2BIG\n");break;

            case EACCES : fprintf(stderr, "EACCES\n");break;

            case EAGAIN : fprintf(stderr, "EAGAIN\n");break;

            case EFAULT : fprintf(stderr, "EFAULT\n");break;

            case EFBIG : fprintf(stderr, "EFBIG\n");break;

            case EIDRM : fprintf(stderr, "EIDRM\n");break;

            case EINTR  : fprintf(stderr, "EINTR\n");break;

            case EINVAL : fprintf(stderr, "EINVAL\n");break;

            case ENOMEM : fprintf(stderr, "ENOMEM\n");break;

            case  ERANGE: fprintf(stderr, "ERANGE\n");break;

            default: fprintf(stderr,"Chai pas\n");break;
        }
        perror("semop P");
        exit(EXIT_FAILURE);
    }
}

/**
 * V(S) avec S un sémaphore.
 * @param sem     le sémaphore concerne
 * @param semid   le tableau de semaphore
 * @return void
 */
void Vas(int sem, int semid) {
    struct sembuf op;

    /* Réalisation de V(Sxx) */
    op.sem_num = sem;
    op.sem_op = 1;
    op.sem_flg = 0;
    if(semop(semid, &op, 1) == -1) {
        fprintf(stderr, "Erreur lors de l'opération sur le sémaphore Vas(%d)\n", sem);
        
        switch(errno){
            case E2BIG : fprintf(stderr, "E2BIG\n");break;

            case EACCES : fprintf(stderr, "EACCES\n");break;

            case EAGAIN : fprintf(stderr, "EAGAIN\n");break;

            case EFAULT : fprintf(stderr, "EFAULT\n");break;

            case EFBIG : fprintf(stderr, "EFBIG\n");break;

            case EIDRM : fprintf(stderr, "EIDRM\n");break;

            case EINTR  : fprintf(stderr, "EINTR\n");break;

            case EINVAL : fprintf(stderr, "EINVAL\n");break;

            case ENOMEM : fprintf(stderr, "ENOMEM\n");break;

            case  ERANGE: fprintf(stderr, "ERANGE\n");break;

            default: fprintf(stderr,"Chai pas\n");break;
        }
        perror("semop V");

        exit(EXIT_FAILURE);
    }
    
}

void swap_poiss(objet_t * obj1, objet_t * obj2, int mode){
                   /* ancien */   /*nouveau*/
    /*test ligne -> lignes ne bougent pas : le type change ou pas*/
    /* echange type obj*/
    obj2->typeObjet=obj1->typeObjet;
    obj1->typeObjet=VIDE;
    /* echange type poiss*/
    obj2->typePoisson=obj1->typePoisson;
    obj1->typePoisson=0;
    /* echange id poiss*/
    obj2->idPoiss=obj1->idPoiss;
    obj1->idPoiss=-1;
    /* echange id Joueur -> requin*/
    obj2->idJoueur=obj1->idJoueur;
    obj1->idJoueur=-1;
    /* echange thread poiss*/
    if(mode == 1){ /* si on est dans le serveur */
        obj2->threadPoisson=obj1->threadPoisson;
        obj1->threadPoisson=0;
    }
}

void kill_poiss(objet_t * obj, int mode){
                   /* ancien */   /*nouveau*/
    obj->typeObjet=VIDE;
    obj->idPoiss=-1;
    obj->typePoisson=0;
    /* echange thread poiss*/
    if(mode == 1){ /* si on est dans le serveur */
        pthread_cancel(obj->threadPoisson);
    }
}