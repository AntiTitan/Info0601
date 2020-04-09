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

/* ON VERRA POUR CA*/
int * enfuite, *libre; /*malloc dans le prog principal*/
void stopPoisson(int sig){

    if(sig== SIGINT){
        boucle=0;
		
		delwin(fen_box_sim);
		delwin(fen_sim);
		delwin(fen_box_msg);
		delwin(fen_msg);
		
		simulation_stopper();
    }
	exit(EXIT_SUCCESS);
}

struct sigaction action;
action.sa_handler = stopPoisson;
sigemptyset(&action.sa_mask);
action.sa_flags = 0;
if(sigaction(SIGINT, &action, NULL) == -1) {
	fprintf(stderr, "Erreur lors du positionnement\n");
	exit(EXIT_FAILURE);
}

/* A GERER A LA FIN AVEC LES CON ET TOUT */

/* dans serveur -> remmetrre ça pour relancer bouger */
pthread_mutex_lock(&abouges);
for(i=0;i<=max_poiss;i++){
	abouge[i]=0;
}
pthread_mutex_unlock(&abouges);


abouge=malloc((max_poiss+1)*sizeof(int));
/* dans joueur -> remmetrre ça pour relancer bouger */
 ncurses_souris(); 
	
	while((ch = getch()) != KEY_F(2) && boucle){ */
		/*gère clic pour la ligne*/
		/* un thread pour affichage poissons 
		et le prog principal qui gère la ligne
			-> mutex sur fen_sim ?*/

	}



int main(int argc, char * argv []){
	
	


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