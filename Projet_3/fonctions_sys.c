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
    obj->idJoueur=-1;
    obj->typePoisson=0;
    /* echange thread poiss*/
    if(mode == 1){ /* si on est dans le serveur */
        pthread_cancel(obj->threadPoisson);
    }
}

void creer_fenetre_box_sim(WINDOW * fen_box_sim,int hauteur, int largeur) {
/*Creation de la fenetre de contour de la fenetre de simulation */
	fen_box_sim = newwin(hauteur + 2, largeur + 2, 0, 0);
	box(fen_box_sim, 0, 0);
	wbkgd(fen_box_sim, COLOR_PAIR(1));
	mvwprintw(fen_box_sim, 0, 3, "SIMULATION");	
	wrefresh(fen_box_sim);
}

void creer_fenetre_sim(WINDOW * fen_sim,WINDOW * fen_box_sim,int hauteur, int largeur) {
/* Creation de la fenetre de simulation dans la fenetre de contour */
/* La simulation est affichee dans cette fenetre */
	fen_sim = subwin(fen_box_sim,hauteur, largeur, 1, 1);
	/* Colore le fond de la fenêtre */
    wbkgd(fen_sim, COLOR_PAIR(2));
    wrefresh(fen_sim);
}

void creer_fenetre_box_msg(WINDOW *fen_box_msg,int largeur) {
/* Creation de la fenetre de contour de la fenetre de messages */
	fen_box_msg = newwin(HAUTEUR_MSG + 2, LARGEUR_MSG + 2, 0, largeur + 2);
	box(fen_box_msg, 0, 0);
	wbkgd(fen_box_msg, COLOR_PAIR(1));
	mvwprintw(fen_box_msg, 0, (largeur + 2) / 2 - 4, "MESSAGES");
	wrefresh(fen_box_msg);
}

void creer_fenetre_msg(WINDOW *fen_msg , WINDOW *fen_box_msg, int largeur) {
/* Creation de la fenetre de messages dans la fenetre de contour */
/* Les messages indicatifs des evenements de la simulation et de l'interface */
/* utilisateur sont affiches dans cete fenetre */
	fen_msg = subwin(fen_box_msg, HAUTEUR_MSG,LARGEUR_MSG, 1, largeur + 3);
	scrollok(fen_msg, TRUE);
	wbkgd(fen_msg, COLOR_PAIR(5));
    wrefresh(fen_msg);
}

void creer_fenetre_box_obj(WINDOW *fen_box_obj, int largeur) {
/* Creation de la fenetre de contour de la fenetre de messages */
	fen_box_obj = newwin(HAUTEUR_MSG + 2, LARGEUR_MSG + 2, HAUTEUR_MSG + 2, largeur + 2);
	box(fen_box_obj, 0, 0);
	wbkgd(fen_box_obj, COLOR_PAIR(1));
	mvwprintw(fen_box_obj, 0, (largeur + 2) / 2 - 4, "OBJETS");
	wrefresh(fen_box_obj);
}

void creer_fenetre_obj(WINDOW * fen_obj, WINDOW *fen_box_obj,int hauteur, int largeur){
    int i;
/* Creation de la fenetre de messages dans la fenetre de contour */
/* Les messages indicatifs des evenements de la simulation et de l'interface */
/* utilisateur sont affiches dans cete fenetre */
	fen_obj = subwin(fen_box_obj, HAUTEUR_MSG,LARGEUR_MSG, HAUTEUR_MSG + 3, largeur + 3);
	/*scrollok(fen_obj, TRUE);*/
	wbkgd(fen_obj, COLOR_PAIR(1));
    /*definition des zones*/
    for(i=0;i<6;i++){
        wattron(fen_obj,COLOR_PAIR(0)); /*zone pneu*/
        mvwprintw(fen_obj,1+i,1,"            ");

        wattroff(fen_obj,COLOR_PAIR(0));
        

        wattron(fen_obj,COLOR_PAIR(4)); /*zone requin*/
        mvwprintw(fen_obj,8+i,1,"            ");

        wattroff(fen_obj,COLOR_PAIR(4));
        

        wattron(fen_obj,COLOR_PAIR(6)); /*zone dynamite*/
        mvwprintw(fen_obj,1+i,16,"            ");

        wattroff(fen_obj,COLOR_PAIR(6));
        

        wattron(fen_obj,COLOR_PAIR(5)); /*zone furtive*/
        mvwprintw(fen_obj,8+i,16,"            ");
        wattroff(fen_obj,COLOR_PAIR(5));
    }
    
    wrefresh(fen_obj);
}
