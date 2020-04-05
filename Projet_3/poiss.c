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

int max_poiss,nbpoiss,action=0;
int abouge;
pthread_mutex_t poissons = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t abouges = PTHREAD_MUTEX_INITIALIZER;
int * enfuite, *libre; /*malloc dans le prog principal*/
pthread_t * * threads_poissons;
pthread_t affiche;
pthread_cond_t aff = PTHREAD_COND_INITIALIZER;
pthread_cond_t bouge = PTHREAD_COND_INITIALIZER;
pthread_mutex_t act = PTHREAD_MUTEX_INITIALIZER;
WINDOW *fen_sim;							/* Fenetre de simulation partagee par les poissons*/
joueur_t joueur;

grille_t etang;

void simulation_initialiser() {
	int i, j;
	
    threads_poissons=malloc(max_poiss*sizeof(pthread_t));
	for (i = 0; i < max_poiss; i++)		/* Au depart il n'y a aucune fourmi dans la simulation */
		
        threads_poissons[i] = NULL;
		
	for (i = 0; i < etang.hauteur; i++) {	/* Initialisation des cases de la simulation */
		for (j = 0; j < etang.largeur; j++) {
			etang.objet[i][j].typeObjet = VIDE;
			etang.objet[i][j].threadPoisson = 0;
			pthread_mutex_init(&etang.objet[i][j].mutObj, NULL);
		}
	}

}

void simulation_stopper() {
	int i;
	
	for (i = 0; i <max_poiss; i++) {
		if (threads_poissons[i] != NULL) {
			pthread_cancel(*threads_poissons[i]);
			threads_poissons[i] = NULL;
		}
	}
}

WINDOW *creer_fenetre_box_sim() {
/*Creation de la fenetre de contour de la fenetre de simulation */

	WINDOW *fen_box_sim;
	
	fen_box_sim = newwin(etang.hauteur + 2, etang.largeur + 2, 0, 0);
	box(fen_box_sim, 0, 0);
	mvwprintw(fen_box_sim, 0, (etang.largeur + 2) / 2 - 5, "SIMULATION");	
	wrefresh(fen_box_sim);
	
	return fen_box_sim;
}

WINDOW *creer_fenetre_sim() {
/* Creation de la fenetre de simulation dans la fenetre de contour */
/* La simulation est affichee dans cette fenetre */

	WINDOW *fen_sim;
	
	fen_sim = newwin(etang.hauteur, etang.largeur, 1, 1);
	
	return fen_sim;
}
void* GestionPref(void* arg) {


    while(1) {
        pthread_mutex_lock(&act);
        /* variable bloquée */

        if (action == BOUGE) {
            pthread_cond_broadcast(&bouge);
        }

        if (action == AFFICHE) {
            pthread_cond_broadcast(&aff);
        }

        /* variable débloquée */
        pthread_mutex_unlock(&act);
        sleep(5);
    }

    return NULL;
}

objet_t creerPoisson(){
	int x,y,place=0,id,type;
	pthread_mutex_lock(&poissons);
	id=nbpoiss;
	nbpoiss++;
	pthread_mutex_unlock(&poissons);
	srand(time(NULL));
	while(!place){
		x = (rand() % etang.hauteur) ;
		y = (rand() % etang.largeur) ;
		pthread_mutex_lock(&etang.objet[x][y].mutObj);
		if(etang.objet[x][y].typeObjet==VIDE){
			etang.objet[x][y].typeObjet=POISSON;
			etang.objet[x][y].idPoiss=id;
			type=(rand () % 100);
			if(type<50){
				etang.objet[x][y].typePoisson=POISSON1;
			}
			else if(type<80){
				etang.objet[x][y].typePoisson=POISSON2;
			}
			else{
				etang.objet[x][y].typePoisson=POISSON3;
			}
			place=1;
		}
		pthread_mutex_unlock(&etang.objet[x][y].mutObj);
	}
	return (etang.objet[x][y]);
}

void *routine_poisson(void *arg) {
	objet_t *obj = (objet_t *) arg;
	int dir, x, y, change,id;
    /*tableau int enfuite (-> poisson en fuite) et tab int continue (-> poisson attrapé)*/
	x = obj->position[0];
	y = obj->position[1];
    id=etang.objet[x][y].idPoiss;
	srand(time(NULL));
	id++;
	id--;
	while (1) { /*s'arrete quand il est tué ou on met une variable -> comment la modifier */

		/*tester s'il y a une ligne proche -> comment faire pour la fuite ?*/
		pthread_mutex_lock(&act);
		
		/*attendre signal deplacement cond*/
		while(action!= BOUGE){
			pthread_cond_wait(&bouge,&act);
		}
		pthread_mutex_unlock(&act);
		pthread_mutex_lock(&etang.objet[y][x].mutObj);
		/*sinon se déplacer*/
		dir = (rand() % 4) ;
		change = 0;
		pthread_testcancel();
		switch(dir) {
			case 1:/*En haut*/
				pthread_mutex_lock(&etang.objet[y-1][x].mutObj);
				pthread_mutex_lock(&etang.objet[y][x].mutObj);
				if (etang.objet[y-1][x].typeObjet == VIDE) {
					etang.objet[y-1][x].typeObjet = obj->typeObjet;
					etang.objet[y][x].typeObjet = VIDE;
					etang.objet[y-1][x].threadPoisson = etang.objet[y][x].threadPoisson;
					etang.objet[y][x].threadPoisson = 0;
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
			case 2:/*A droite*/
				pthread_mutex_lock(&etang.objet[y][x+1].mutObj);
				pthread_mutex_lock(&etang.objet[y][x].mutObj);
				if (etang.objet[y][x+1].typeObjet == VIDE) {
					etang.objet[y][x+1].typeObjet = obj->typeObjet;
					etang.objet[y][x].typeObjet = VIDE;
					etang.objet[y][x+1].threadPoisson = etang.objet[y][x].threadPoisson;
					etang.objet[y][x].threadPoisson = 0;
					/* envoi de la nouvelle coordonnées
						idPoiss, position 
					*/
					/*
					mvwprintw(fen_sim, y, x, " ");  fond bleu 
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss);  fond jaune avec id Poisson
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
			case 3:/*En bas*/
				pthread_mutex_lock(&etang.objet[y+1][x].mutObj);
				pthread_mutex_lock(&etang.objet[y][x].mutObj);
				if (etang.objet[y+1][x].typeObjet == VIDE) {
					etang.objet[y+1][x].typeObjet = obj->typeObjet;
					etang.objet[y][x].typeObjet = VIDE;
					etang.objet[y+1][x].threadPoisson = etang.objet[y][x].threadPoisson;
					etang.objet[y][x].threadPoisson = 0;
					/* envoi de la nouvelle coordonnées
						idPoiss, position 
					*/
					/*
					mvwprintw(fen_sim, y, x, " ");  fond bleu 
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss);  fond jaune avec id Poisson
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
			case 4:/*A gauche*/
				pthread_mutex_lock(&etang.objet[y][x-1].mutObj);
				pthread_mutex_lock(&etang.objet[y][x].mutObj);
				if (etang.objet[y][x-1].typeObjet == VIDE) {
					etang.objet[y][x-1].typeObjet = obj->typeObjet;
					etang.objet[y][x].typeObjet = VIDE;
					etang.objet[y][x-1].threadPoisson = etang.objet[y][x].threadPoisson;
					etang.objet[y][x].threadPoisson = 0;
					/* envoi de la nouvelle coordonnées
						idPoiss, position 
					*/
					/*
					mvwprintw(fen_sim, y, x, " "); fond bleu 
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss);  fond jaune avec id Poisson
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
		wrefresh(fen_sim);
		pthread_mutex_lock(&abouges);
		abouge++;

		pthread_mutex_lock(&poissons);
		if(abouge==nbpoiss){ /* c'est le dernier poisson qui bouge qui lance l'affichage */
			pthread_mutex_lock(&act);
			action=AFFICHE;
			pthread_mutex_unlock(&act);
		}
		pthread_mutex_unlock(&poissons);
		pthread_mutex_unlock(&abouges);
		
		/* c'est pas comme ça qu'on va afficher une fois par tour
			-> bloquer threadPoiss au bout de 1 tour et afficher ?
					si oui, comment ?*/
	}
	
	free(obj);
	return NULL;
}

void * affichage(void * arg){
	int i,j;
	while(1){
		/* on bloque tous les mutex au départ -> pas besoin si on fait affichage tour par tour */
		pthread_mutex_lock(&act);
		/*
		for(i=0;i<etang.hauteur;i++){
			for(j=0;j<etang.largeur;j++){
				pthread_mutex_lock(&etang.objet[i][j].mutObj);
			}
		}
		*/
		while(action != AFFICHE){
			pthread_cond_wait(&aff, &act);
		}
		pthread_mutex_unlock(&act);

		for(i=0;i<etang.hauteur;i++){
			for(j=0;j<etang.largeur;j++){
				pthread_mutex_lock(&etang.objet[i][j].mutObj);
				switch(etang.objet[i][j].typeObjet){
					case(POISSON) :
						
					mvwprintw(fen_sim, j, i, " "); /* fond bleu */
					mvwprintw(fen_sim, j+1, i, "%d",etang.objet[i][j].idPoiss); /* fond jaune avec id Poisson */
					
					break;
					case(REQUIN) :
						/*tester idJoueur du requin*/
						mvwprintw(fen_sim, j, i, " "); /* fond bleu */
						if(etang.objet[i][j].idJoueur==joueur.idJoueur){
							mvwprintw(fen_sim, j+1, i, " "); /* fond vert */
						}
						else{
							mvwprintw(fen_sim, j+1, i, "%d", etang.objet[i][j].idPoiss); /* fond jaune avec id Poisson */
						}
						/* fond jaune avec idPoiss en noir 
							ou
						   fond vert*/
					break;
					case(VIDE) :
						mvwprintw(fen_sim, j, i, " "); /* fond bleu */
					break;
					case(DYNAMITE) :
						/* je ne sais pas s'il y a besoin,
						 ça explose direct ou pas ?*/
					break;
					case(PNEU) :
						/*tester idJoueur du pneu*/
						if(etang.objet[i][j].idJoueur==joueur.idJoueur){
							mvwprintw(fen_sim, j+1, i, " "); /* fond noir */
						}
						else{
							mvwprintw(fen_sim, j+1, i, " "); /* fond bleu */
						}
						/*fond noir ou bleu selon idJ*/
					break;
					case(LIGNE) :
						/*tester idJoueur de la ligne*/
						if(etang.objet[i][j].idJoueur==joueur.idJoueur){
							mvwprintw(fen_sim, j+1, i, " "); /* fond gris ou violet */
						}
						else{
							mvwprintw(fen_sim, j+1, i, " "); /* fond bleu */
						}
						/*fond gris ou bleu selon idJ*/
					break;
				}
				pthread_mutex_lock(&etang.objet[i][j].mutObj);
				sleep(3);
				pthread_mutex_lock(&act);
				action=BOUGE;
				pthread_mutex_unlock(&act);
			}
		}
		/*
		for(i=0;i<etang.hauteur;i++){
			for(j=0;j<etang.largeur;j++){
				pthread_mutex_unlock(&etang.objet[i][j].mutObj);
			}
		}
		*/
	}
}

int main(int argc, char * argv []){
	WINDOW *fen_box_sim;
	pthread_t gerant;
	objet_t * obj;
	int ch, i,j,statut;

    if(argc!=3){
        fprintf(stderr, "Usage : %s dimensions (L | H)\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " dimensions : largeur et hauteur de la zone de jeu\n");
        exit(EXIT_FAILURE);
    }
    etang.largeur=atoi(argv[1]);
    etang.hauteur=atoi(argv[2]);
    max_poiss= (etang.largeur*etang.hauteur)/5;

	abouge=0;
	fen_box_sim = creer_fenetre_box_sim();
	fen_sim = creer_fenetre_sim();

    for(i=0;i<etang.hauteur;i++){
        etang.objet[i]=malloc(sizeof(objet_t)*etang.largeur);
        for(j=0;j<etang.largeur;j++){
            etang.objet[i][j].typeObjet=VIDE;
        }
    }

	ncurses_initialiser();
	simulation_initialiser();
	ncurses_souris();
	statut = pthread_create(&gerant, NULL, GestionPref, NULL);
	
	for(i=0;i<max_poiss/5;i++){
		obj=(objet_t*)malloc(sizeof(objet_t));
		*obj=creerPoisson();
		statut=pthread_create(threads_poissons[i], NULL,routine_poisson,(void *)obj);
		if(statut!=0){
			printf("Pb création threadpoiss %d\n",i);
		}
	}
	statut=pthread_create(&affiche, NULL,affichage,NULL);
	mvprintw(LINES - 1, 0, "Tapez F2 pour quitter");
	wrefresh(stdscr);
	while((ch = getch()) != KEY_F(2)){
		/*gère clic pour la ligne*/
		/* un thread pour affichage poissons 
		et le prog principal qui gère la ligne
			-> mutex sur fen_sim ?*/

	}

	delwin(fen_box_sim);
	delwin(fen_sim);
	simulation_stopper();
	ncurses_stopper();
	for(i=0;i<etang.hauteur;i++){
        free(etang.objet[i]); 
    }
	free(threads_poissons);
    return EXIT_SUCCESS;
}