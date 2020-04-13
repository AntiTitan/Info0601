#define _XOPEN_SOURCE
#include "struct_message.h"
#include "fonctions_sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
#include "ncurses.h"

#define BOUGE 1
#define AFFICHE 2

typedef struct cp {
	int x,y;
} coordp_t;

int max_poiss,nbpoiss,actPoiss=0;
int * abouge,boucle=1;
pthread_mutex_t poissons = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t abouges = PTHREAD_MUTEX_INITIALIZER;
int * enfuite, *libre; /*malloc dans le prog principal*/
pthread_t * * threads_poissons;
pthread_t affiche;
pthread_t gerant;
pthread_cond_t aff = PTHREAD_COND_INITIALIZER;
pthread_cond_t bouge = PTHREAD_COND_INITIALIZER;
pthread_mutex_t act = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mess = PTHREAD_MUTEX_INITIALIZER;
WINDOW *fen_sim,*fen_msg;
WINDOW *fen_box_sim,*fen_box_msg;							/* Fenetre de simulation partagee par les poissons*/
joueur_t joueur;

grille_t etang;

void simulation_initialiser() {
	int i, j;
	
    threads_poissons=malloc(max_poiss*sizeof(pthread_t *));
	for (i = 0; i < max_poiss; i++){		/* Au depart il n'y a aucun poisson dans la simulation */
        threads_poissons[i] = malloc(sizeof(pthread_t));
	}
	for (i = 0; i < etang.hauteur; i++) {	/* Initialisation des cases de la simulation */
		for (j = 0; j < etang.largeur; j++) {
			/* etang.objet[i][j].threadPoisson = 0; */
			
			etang.objet[i][j].typeObjet = VIDE;
			pthread_mutex_init(&etang.objet[i][j].mutObj, NULL);
		}
	}
	
}

void simulation_stopper() {
	int i;
	
	pthread_mutex_lock(&poissons);
	for(i=0;i<nbpoiss;i++){
		pthread_cancel(*threads_poissons[i]);
		free(threads_poissons[i]);
	}
	pthread_mutex_unlock(&poissons);
	pthread_cancel(affiche);
	pthread_cancel(gerant);
	free(threads_poissons);
	free(abouge);
	for(i=0;i<etang.hauteur;i++){
		free(etang.objet[i]);
	}
	free(etang.objet);
	free(fen_msg);
	free(fen_sim);
	free(fen_box_msg);
	free(fen_box_sim);
}

void creer_fenetre_box_simp() {
/*Creation de la fenetre de contour de la fenetre de simulation */
	
	fen_box_sim = newwin(etang.hauteur + 2, etang.largeur + 2, 0, 0);
	box(fen_box_sim, 0, 0);
	wbkgd(fen_box_sim, COLOR_PAIR(1));
	mvwprintw(fen_box_sim, 0, 1, "SIMULATION");	
	wrefresh(fen_box_sim);
}

void creer_fenetre_simp() {
/* Creation de la fenetre de simulation dans la fenetre de contour */
/* La simulation est affichee dans cette fenetre */
	fen_sim = subwin(fen_box_sim,etang.hauteur, etang.largeur, 1, 1);
	/* Colore le fond de la fenêtre */
    wbkgd(fen_sim, COLOR_PAIR(2));
    wrefresh(fen_sim);
}

void creer_fenetre_box_msgp() {
/* Creation de la fenetre de contour de la fenetre de messages */
	fen_box_msg = newwin(15 + 2, 30 + 2, 0, etang.largeur + 2);
	box(fen_box_msg, 0, 0);
	wbkgd(fen_box_msg, COLOR_PAIR(1));
	mvwprintw(fen_box_msg, 0, (etang.largeur + 2) / 2 - 4, "MESSAGES");
	wrefresh(fen_box_msg);
}

void creer_fenetre_msgp() {
/* Creation de la fenetre de messages dans la fenetre de contour */
/* Les messages indicatifs des evenements de la simulation et de l'interface */
/* utilisateur sont affiches dans cete fenetre */
	fen_msg = subwin(fen_box_msg, 15,30, 1, etang.largeur + 3);
	scrollok(fen_msg, TRUE);
	wbkgd(fen_msg, COLOR_PAIR(5));
    wrefresh(fen_msg);
}


void* GestionPref(void* arg) {


    while(1) {
        pthread_mutex_lock(&act);
        /* variable bloquée */

        if (actPoiss == BOUGE) {
            pthread_cond_broadcast(&bouge);
        }

        if (actPoiss == AFFICHE) {
            pthread_cond_broadcast(&aff);
        }

        /* variable débloquée */
        pthread_mutex_unlock(&act);
        sleep(1);
    }

    return NULL;
}

void creerPoisson(coordp_t * coord){
	int x,y,place=0,id,type;
	
	pthread_mutex_lock(&poissons);
	id=nbpoiss;
	
	nbpoiss++;
	pthread_mutex_unlock(&poissons);
	
	/*
	pthread_mutex_lock(&mess);
	mvwprintw(fen_msg, 0, 0, "poisson %d\n",id);    fond gris ou violet *
	pthread_mutex_unlock(&mess); */
	srand(time(NULL));
	while(!place){
		
		y = (rand() % etang.hauteur) ;
		x = (rand() % etang.largeur) ;
		pthread_mutex_lock(&etang.objet[y][x].mutObj);
		
		if(etang.objet[y][x].typeObjet==VIDE){
			etang.objet[y][x].typeObjet=POISSON;
			etang.objet[y][x].idPoiss=id;
			type=(rand () % 100);
			if(type<50){
				etang.objet[y][x].typePoisson=POISSON1;
			}
			else if(type<80){
				etang.objet[y][x].typePoisson=POISSON2;
			}
			else{
				etang.objet[y][x].typePoisson=POISSON3;
			}
			place=1;
		}
		pthread_mutex_unlock(&etang.objet[y][x].mutObj);
	}
	pthread_mutex_lock(&mess);
	mvwprintw(fen_msg,id,0,"poiss %d, pos (%d,%d), t %d\n",id,x,y,type);
	wrefresh(fen_msg);
	pthread_mutex_unlock(&mess);
	coord->y=y;
	coord->x=x;
}

void *routine_poisson(void *arg) {
	coordp_t *coord = (coordp_t *) arg;
	int dir, x, y, change,id,unlock=0;
    /*tableau int enfuite (-> poisson en fuite) et tab int continue (-> poisson attrapé)*/
	sleep(3);
	y = coord->y;
	x = coord->x;
	pthread_mutex_lock(&etang.objet[y][x].mutObj);
    id=etang.objet[y][x].idPoiss;
	pthread_mutex_unlock(&etang.objet[y][x].mutObj);
	srand(time(NULL));
	while (boucle) { /*s'arrete quand il est tué ou on met une variable -> comment la modifier */

		/*tester s'il y a une ligne proche -> comment faire pour la fuite ?*/
		pthread_mutex_lock(&act);
		
		/*attendre signal deplacement cond*/
		while(actPoiss != BOUGE){
			pthread_cond_wait(&bouge,&act);
		}
		pthread_mutex_unlock(&act); /*Il faut peut etre le mettre a la fin*/
		
		pthread_mutex_lock(&abouges);
		unlock=0;
		if(abouge[id]==0){
			pthread_mutex_unlock(&abouges);
			unlock=1;
			/*sinon se déplacer*/
			dir = (rand() % 4) ;
			change = 0;
			pthread_testcancel();
			switch(dir) {
				case 0:/*En haut*/
					if(y>0){
						pthread_mutex_lock(&etang.objet[y-1][x].mutObj);
						pthread_mutex_lock(&etang.objet[y][x].mutObj);
						if (etang.objet[y-1][x].typeObjet == VIDE) {
							etang.objet[y-1][x].typeObjet = etang.objet[y][x].typeObjet;
							etang.objet[y][x].typeObjet = VIDE;
							etang.objet[y-1][x].threadPoisson = etang.objet[y][x].threadPoisson;
							etang.objet[y][x].threadPoisson = 0;
							etang.objet[y-1][x].typePoisson = etang.objet[y][x].typePoisson;
							etang.objet[y][x].typePoisson = 0;
							etang.objet[y-1][x].idPoiss = etang.objet[y][x].idPoiss;
							etang.objet[y][x].idPoiss = -1;
							/* envoi de la nouvelle coordonnées
								idPoiss, position 
							*/
							y--;
							change = 1;
						}
						if (change) {
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
							pthread_mutex_unlock(&etang.objet[y+1][x].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang.objet[y-1][x].mutObj);
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
						}
						break;
					}
					
				case 1:/*A droite*/
					if(x<etang.largeur-1){
						pthread_mutex_lock(&etang.objet[y][x+1].mutObj);
						pthread_mutex_lock(&etang.objet[y][x].mutObj);
						if (etang.objet[y][x+1].typeObjet == VIDE) {
							etang.objet[y][x+1].typeObjet = etang.objet[y][x].typeObjet;
							etang.objet[y][x].typeObjet = VIDE;
							etang.objet[y][x+1].threadPoisson = etang.objet[y][x].threadPoisson;
							etang.objet[y][x].threadPoisson = 0;
							etang.objet[y][x+1].typePoisson = etang.objet[y][x].typePoisson;
							etang.objet[y][x].typePoisson = 0;

							etang.objet[y][x+1].idPoiss = etang.objet[y][x].idPoiss;
							etang.objet[y][x].idPoiss = -1;
							/* envoi de la nouvelle coordonnées
								idPoiss, position 
							*/
							x++;
							change = 1;
						}
						if (change) {
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
							pthread_mutex_unlock(&etang.objet[y][x-1].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang.objet[y][x+1].mutObj);
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
						}
						break;
					}
				case 2:/*En bas*/
					if(y<etang.hauteur-1){
						pthread_mutex_lock(&etang.objet[y+1][x].mutObj);
						pthread_mutex_lock(&etang.objet[y][x].mutObj);
						if (etang.objet[y+1][x].typeObjet == VIDE) {
							etang.objet[y+1][x].typeObjet = etang.objet[y][x].typeObjet;
							etang.objet[y][x].typeObjet = VIDE;
							etang.objet[y+1][x].threadPoisson = etang.objet[y][x].threadPoisson;
							etang.objet[y][x].threadPoisson = 0;
							etang.objet[y+1][x].typePoisson = etang.objet[y][x].typePoisson;
							etang.objet[y][x].typePoisson = 0;
							etang.objet[y+1][x].idPoiss = etang.objet[y][x].idPoiss;
							etang.objet[y][x].idPoiss = -1;
							/* envoi de la nouvelle coordonnées
								idPoiss, position 
							*/
							y++;
							change = 1;
						}
						if (change) {
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
							pthread_mutex_unlock(&etang.objet[y-1][x].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang.objet[y+1][x].mutObj);
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
						}
						break;
					}
				case 3:/*A gauche*/
					if(x!=0){
						pthread_mutex_lock(&etang.objet[y][x-1].mutObj);
						pthread_mutex_lock(&etang.objet[y][x].mutObj);
						if (etang.objet[y][x-1].typeObjet == VIDE) {
							etang.objet[y][x-1].typeObjet = etang.objet[y][x].typeObjet;
							etang.objet[y][x].typeObjet = VIDE;
							etang.objet[y][x-1].threadPoisson = etang.objet[y][x].threadPoisson;
							etang.objet[y][x].threadPoisson = 0;
							etang.objet[y][x-1].typePoisson = etang.objet[y][x].typePoisson;
							etang.objet[y][x].typePoisson = 0;
							etang.objet[y][x-1].idPoiss = etang.objet[y][x].idPoiss;
							etang.objet[y][x].idPoiss = -1;
							/* envoi de la nouvelle coordonnées
								idPoiss, position 
							*/
							x--;
							change = 1;
						}
						if (change) {
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
							pthread_mutex_unlock(&etang.objet[y][x+1].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang.objet[y][x-1].mutObj);
							pthread_mutex_unlock(&etang.objet[y][x].mutObj);
						}
						break;
					}
			}

			pthread_mutex_lock(&abouges);
			abouge[max_poiss]++;
			abouge[id]=1;
			pthread_mutex_lock(&poissons);
			if(abouge[max_poiss]==nbpoiss){ /* c'est le dernier poisson qui bouge qui lance l'affichage */
				pthread_mutex_lock(&act);
				actPoiss=AFFICHE;
				pthread_mutex_unlock(&act);
			}

			pthread_mutex_unlock(&poissons);
			pthread_mutex_unlock(&abouges);
		}
		if(unlock==0){
			pthread_mutex_unlock(&abouges);
		}
		/* c'est pas comme ça qu'on va afficher une fois par tour
			-> bloquer threadPoiss au bout de 1 tour et afficher ?
					si oui, comment ?*/
	}
	return NULL;
}

void * affichage(void * arg){
	int i,j;
	while(boucle){
		/* on fait affichage tour par tour */
		pthread_mutex_lock(&act);

		while(actPoiss != AFFICHE){
			pthread_cond_wait(&aff, &act);
		}
		pthread_mutex_unlock(&act);

		for(i=0;i<etang.hauteur;i++){
			for(j=0;j<etang.largeur;j++){
				pthread_mutex_lock(&etang.objet[i][j].mutObj);				
				switch(etang.objet[i][j].typeObjet){
					case(POISSON) :
						wattron(fen_sim,COLOR_PAIR(3));
						mvwprintw(fen_sim, i, j, "%d",etang.objet[i][j].typePoisson); /* fond jaune avec type Poisson */
						wattroff(fen_sim,COLOR_PAIR(3));
						break;
					case(REQUIN) :
						/*tester idJoueur du requin*/
						/* fond bleu */
						if(etang.objet[i][j].idJoueur==joueur.idJoueur){
							wattron(fen_sim,COLOR_PAIR(4));
							mvwprintw(fen_sim, i, j, " ");  /* fond vert */
							wattroff(fen_sim,COLOR_PAIR(4));
						}
						else{
							wattron(fen_sim,COLOR_PAIR(3));
							mvwprintw(fen_sim, i, j, "%d", etang.objet[i][j].idPoiss);  /* fond jaune avec type Poisson */
							wattroff(fen_sim,COLOR_PAIR(3));
						}
						/* fond jaune avec typePoiss en noir 
							ou
						   fond vert*/
						break;
					case(VIDE) :
						mvwprintw(fen_sim, i, j, " "); /* fond bleu */
						break;
					case(DYNAMITE) :
						/* je ne sais pas s'il y a besoin,
						 ça explose direct ou pas ?*/
						break;
					case(PNEU) :
						/*tester idJoueur du pneu*/
						if(etang.objet[i][j].idJoueur==joueur.idJoueur){
							wattron(fen_sim,COLOR_PAIR(1));
							mvwprintw(fen_sim, i, j, " "); /* fond noir */
							wattroff(fen_sim,COLOR_PAIR(1));
						}
						else{
							mvwprintw(fen_sim, i, j, " "); /* fond bleu */
						}
						/*fond noir ou bleu selon idJ*/
						break;
				}
				pthread_mutex_unlock(&etang.objet[i][j].mutObj); 
				
			}
		}
		wrefresh(fen_sim);
		pthread_mutex_lock(&abouges);
		for(i=0;i<=max_poiss;i++){
			abouge[i]=0;
		}
		pthread_mutex_unlock(&abouges);
		pthread_mutex_lock(&act);
		actPoiss=BOUGE;
		pthread_mutex_unlock(&act);
	}
	return NULL;
}

void stopPoisson(int sig){

    if(sig== SIGINT){
        boucle=0;
		
		delwin(fen_box_sim);
		delwin(fen_sim);
		delwin(fen_box_msg);
		delwin(fen_msg);
		
		simulation_stopper();
		
		ncurses_stopper();
    }
	exit(EXIT_SUCCESS);
}

int main(int argc, char * argv []){
	struct sigaction action;
	coordp_t * coord;
	void * res;
	int  i,j,statut;

    if(argc!=3){
        fprintf(stderr, "Usage : %s dimensions (L | H)\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " dimensions : largeur et hauteur de la zone de jeu\n");
        exit(EXIT_FAILURE);
    }
	

	action.sa_handler = stopPoisson;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if(sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Erreur lors du positionnement\n");
        exit(EXIT_FAILURE);
    }

    etang.largeur=atoi(argv[1]);
    etang.hauteur=atoi(argv[2]);
    max_poiss= (etang.largeur*etang.hauteur)/5;

	abouge=malloc((max_poiss+1)*sizeof(int));
	etang.objet=malloc(sizeof(objet_t*)*etang.hauteur);

    for(i=0;i<etang.hauteur;i++){
        etang.objet[i]=(objet_t*)malloc(sizeof(objet_t)*etang.largeur);
		for(j=0;j<etang.largeur;j++){
            etang.objet[i][j].typeObjet=VIDE;
        }
    }

	simulation_initialiser();
	ncurses_initialiser();
	ncurses_couleurs();
	init_pair(0, COLOR_WHITE, COLOR_BLACK); /* pour les pneus et pneu */
    init_pair(1, COLOR_WHITE, COLOR_BLACK); /* pour les box */
    init_pair(2, COLOR_WHITE, COLOR_BLUE); /* pour l'etang */
    init_pair(3, COLOR_BLACK, COLOR_YELLOW); /* pour les poissons */
    init_pair(4, COLOR_BLACK, COLOR_GREEN); /* pour les requins */
	init_pair(5, COLOR_WHITE, COLOR_MAGENTA); /* pour les lignes */
    init_pair(6, COLOR_BLACK, COLOR_RED); /* pour les dynamites */
	pthread_mutex_lock(&abouges);
	for(i=0;i<=max_poiss;i++){
		abouge[i] = 0;
	}
	pthread_mutex_unlock(&abouges);
	
	creer_fenetre_box_simp();
	creer_fenetre_simp();
	creer_fenetre_box_msgp();
	creer_fenetre_msgp();
	
	
	statut = pthread_create(&gerant, NULL, GestionPref, NULL);
	statut = pthread_create(&affiche, NULL,affichage,NULL);
	actPoiss=BOUGE;
	/* printf("max poiss %d\n",max_poiss/5); */
	
	for(i=0;i<max_poiss/5;i++){
		
		coord=(coordp_t*)malloc(sizeof(coordp_t)); 
		
		creerPoisson(coord);
		
		statut=pthread_create(threads_poissons[i], NULL,routine_poisson,(void *)coord);

		if(statut!=0){
			printf("Pb création threadpoiss %d\n",i);
		}
	}
	
	
	
	/* ncurses_souris(); */
	/*
	mvprintw(LINES - 1, 0, "Tapez F2 pour quitter");
	wrefresh(stdscr); */
	/*
	while((ch = getch()) != KEY_F(2) && boucle){ */
		/*gère clic pour la ligne*/
		/* un thread pour affichage poissons 
		et le prog principal qui gère la ligne
			-> mutex sur fen_sim ?*/

	/*} */

	pthread_join(affiche,&res);
	pthread_join(gerant,&res);
	for(i=0;i<nbpoiss;i++){
		pthread_join(*threads_poissons[i],&res);
	}

	delwin(fen_box_sim);
	delwin(fen_sim);
	delwin(fen_box_msg);
	delwin(fen_msg);
	simulation_stopper();
	ncurses_stopper();
    return EXIT_SUCCESS;
}