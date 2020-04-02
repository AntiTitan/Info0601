#define _XOPEN_SOURCE
#include "struct_message.h"
#include "fonctions_sys.h"
#include <ncurses.h>
#include "ncurses.h"



int max_poiss;
int * enfuite, *libre; /*malloc dans le prog principal*/
pthread_t * * threads_poissons;
WINDOW *fen_sim;							/* Fenetre de simulation partagee par les fourmis*/

grille_t etang;

void simulation_initialiser() {
	int i, j;
	
    threads_poissons=malloc(max_poiss*sizeof(pthread_t));
	for (i = 0; i < max_poiss; i++)		/* Au depart il n'y a aucune fourmi dans la simulation */
		
        threads_poissons[i] = NULL;
		
	for (i = 0; i < etang.hauteur; i++) {	/* Initialisation des cases de la simulation */
		for (j = 0; j < etang.largeur; j++) {
			etang.objet[i][j].typeObjet = VIDE;
			etang.objet[i][j].threadPoisson = NULL;
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

void * affichage(void * arg){
	int i,j;
	while(1){

		for(i=0;i<etang.hauteur;i++){
			for(j=0;j<etang.largeur;j++){
				pthread_mutex_lock(&etang.objet[i][j].mutObj);
				switch(etang.objet[i][j].typeObjet){
					case(POISSON) :
						switch(etang.objet[i][j].idPoiss){
							case(POISSON1) :

							break;
							case(POISSON2) :

							break;
							case(POISSON3) :

							break;
							case(P_REQUIN) :
								/*tester idJoueur du requin*/
							break;
						}
					break;
					case(VIDE) :

					break;
					case(DYNAMITE) :
						/* je ne sais pas s'il y a besoin,
						 ça explose direct ou pas ?*/
					break;
					case(PNEU) :
						/*tester idJoueur du pneu*/
					break;
					case(LIGNE) :
						/*tester idJoueur de la ligne*/
					break;
				}
			}
		}
	}
}

void *routine_poisson(void *arg) {
	objet_t *obj = (objet_t *) arg;
	pthread_t* suppr;
	int dir, x, y, change,enfuite=0,id;
    /*tableau int enfuite (-> poisson en fuite) et tab int continue (-> poisson attrapé)*/
	x = obj->position[0];
	y = obj->position[1];
    id=etang.objet[x][y].idPoiss;
	srand(time(NULL));
	while (1) { /*s'arrete quand il est tué ou on met une variable -> comment la modifier */

    /*tester s'il y a une ligne proche -> comment faire pour la fuite ?*/
    pthread_mutex_lock(&etang.objet[y][x].mutObj);

    /* en haut */
    pthread_mutex_lock(&etang.objet[y-1][x].mutObj);
    if(etang.objet[y-1][x].typeObjet == 5 && !enfuite ){
        /*attrapé par etang.objet[y-1][x].idJoueur */
        /* blocage ?*/
    }
    pthread_mutex_unlock(&etang.objet[y-1][x].mutObj);

    /* en bas */
    pthread_mutex_lock(&etang.objet[y+1][x].mutObj);
    if(etang.objet[y+1][x].typeObjet == 5 && !enfuite ){
        /*attrapé par etang.objet[y+1][x].idJoueur */
        /* blocage ?*/
    }
    pthread_mutex_unlock(&etang.objet[y+1][x].mutObj);

    /* à droite */
    pthread_mutex_lock(&etang.objet[y][x+1].mutObj);
    if(etang.objet[y][x+1].typeObjet == 5 && !enfuite ){
        /*attrapé par etang.objet[y][x+1].idJoueur */
        /* blocage ?*/
    }
    pthread_mutex_unlock(&etang.objet[y][x+1].mutObj);

    /* à gauche */
    pthread_mutex_lock(&etang.objet[y][x-1].mutObj);
    if(etang.objet[y][x-1].typeObjet == 5 && !enfuite ){
        /*attrapé par etang.objet[y][x-1].idJoueur */
        /* blocage ?*/
    }
    pthread_mutex_unlock(&etang.objet[y][x-1].mutObj);
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
					etang.objet[y][x].threadPoisson = NULL;
					mvwprintw(fen_sim, y, x, " "); /* fond bleu */
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss); /* fond jaune avec id Poisson*/
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
					etang.objet[y][x].threadPoisson = NULL;
					mvwprintw(fen_sim, y, x, " "); /* fond bleu */
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss); /* fond jaune avec id Poisson*/
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
					etang.objet[y][x].threadPoisson = NULL;
					mvwprintw(fen_sim, y, x, " "); /* fond bleu */
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss); /* fond jaune avec id Poisson*/
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
					etang.objet[y][x].threadPoisson = NULL;
					mvwprintw(fen_sim, y, x, " "); /* fond bleu */
					mvwprintw(fen_sim, y+1, x, etang.objet[y][x].idPoiss); /* fond jaune avec id Poisson*/
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
		sleep(1);
	}
	
	free(obj);
	return NULL;
}

int main(int argc, char * argv []){
	WINDOW *fen_box_sim;
    MEVENT event;
	int ch, i,j;

    if(argc!=3){
        fprintf(stderr, "Usage : %s dimensions (L | H)\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " dimensions : largeur et hauteur de la zone de jeu\n");
        exit(EXIT_FAILURE);
    }
    etang.largeur=atoi(argv[1]);
    etang.hauteur=atoi(argv[2]);
    max_poiss= (etang.largeur*etang.hauteur)/5;

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

    return EXIT_SUCCESS;
}