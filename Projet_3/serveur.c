#define _XOPEN_SOURCE
/*include*/

#include "fonctions_sys.h"
#include "struct_message.h"

int boucle[MAX_PARTIE];
int bouclePrincipale=1;
int semid;
int sockfdUDP;
int fdTCP;
int largeur, hauteur;
int max_poiss,nbpoiss[MAX_PARTIE],actPoiss[MAX_PARTIE];
int * abouge [MAX_PARTIE];
int socketJ [MAX_JOUEURS];
grille_t etang [MAX_PARTIE];

pthread_t threadTCP[MAX_PARTIE];
pthread_t jTCP[MAX_JOUEURS];
pthread_t * * threads_poissons [MAX_PARTIE];
pthread_t gerant[MAX_PARTIE];

pthread_mutex_t nbPoissons [MAX_PARTIE];
pthread_mutex_t abouges [MAX_PARTIE];
pthread_mutex_t act [MAX_PARTIE];
pthread_mutex_t tour [MAX_PARTIE];
pthread_mutex_t sock[MAX_PARTIE];

pthread_cond_t bouge [MAX_PARTIE];

void* GestionAction(void* arg) {
    int * partie = (int*)arg;
    while(boucle[*partie]) {
        pthread_mutex_lock(&act[*partie]);
        /* variable bloquée */

        if (actPoiss[*partie] == BOUGE) {
            pthread_cond_broadcast(&bouge[*partie]);
        }

        /* variable débloquée */
        pthread_mutex_unlock(&act[*partie]);
        sleep(1);
    }

    return NULL;
}

void creerRequin(int y,int x,int partie,int id){
	int idP,type;
    message_t msg;
	pthread_mutex_lock(&nbPoissons[partie]);
	idP=nbpoiss[partie];
	nbpoiss[partie]++;
	pthread_mutex_unlock(&nbPoissons[partie]);
	/*pas besoin de lock le mutex obj*/
	srand(time(NULL));
    etang[partie].objet[y][x].typeObjet=REQUIN;
    etang[partie].objet[y][x].idPoiss=idP;
    etang[partie].objet[y][x].idJoueur=id;
    type=(rand () % 100);
    if(type<50){
        etang[partie].objet[y][x].typePoisson=POISSON1;
    }
    else if(type<80){
        etang[partie].objet[y][x].typePoisson=POISSON2;
    }
    else{
        etang[partie].objet[y][x].typePoisson=POISSON3;
    }
    etang[partie].objet[y][x].etatPoisson=LIBRE;

    msg.typeMessage=C_POISS;
    msg.idPoisson=id;
    msg.typePoisson=etang[partie].objet[y][x].typePoisson;
    msg.position[0]=y;
    msg.position[1]=x;

    pthread_mutex_lock(&sock[partie]);
    if(write(socketJ[partie*2],&msg,sizeof(message_t))==-1){
        perror("Erreur lors de l'envoi de M_POISS au joueur\n");
    }
    if(write(socketJ[partie*2+1],&msg,sizeof(message_t))==-1){
        perror("Erreur lors de l'envoi de M_POISS au joueur\n");
    }
    pthread_mutex_unlock(&sock[partie]);
}

void creerPoisson(coord_t * coord){
	int x,y,place=0,id,type,partie;
    message_t msg;
    partie=coord->partie;
	pthread_mutex_lock(&nbPoissons[partie]);
	id=nbpoiss[partie];
	nbpoiss[partie]++;
	pthread_mutex_unlock(&nbPoissons[partie]);
	
	srand(time(NULL));
    
	while(!place){
		
		y = (rand() % hauteur) ;
		x = (rand() % largeur) ;
		pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
		
		if(etang[partie].objet[y][x].typeObjet==VIDE){
			etang[partie].objet[y][x].typeObjet=POISSON;
			etang[partie].objet[y][x].idPoiss=id;
			type=(rand () % 100);
			if(type<50){
				etang[partie].objet[y][x].typePoisson=POISSON1;
			}
			else if(type<80){
				etang[partie].objet[y][x].typePoisson=POISSON2;
			}
			else{
				etang[partie].objet[y][x].typePoisson=POISSON3;
			}
            etang[partie].objet[y][x].etatPoisson=LIBRE;
			place=1;
		}
    
        msg.typeMessage=C_POISS;
        msg.typePoisson=etang[partie].objet[y][x].typePoisson;
        pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
        msg.idPoisson=id;
        msg.position[0]=y;
        msg.position[1]=x;
        pthread_mutex_lock(&sock[partie]);
        if(write(socketJ[partie*2],&msg,sizeof(message_t))==-1){
            perror("Erreur lors de l'envoi de M_POISS au joueur\n");
        }
        if(write(socketJ[partie*2+1],&msg,sizeof(message_t))==-1){
            perror("Erreur lors de l'envoi de M_POISS au joueur\n");
        }
        pthread_mutex_unlock(&sock[partie]);
	}
	coord->y=y;
	coord->x=x;
}

void chrono_poisson(int partie,int y,int x,int idPoiss){
    pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
    etang[partie].objet[y][x].etatPoisson=MORDU;
    pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
    sleep(5);
    pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
    if(etang[partie].objet[y][x].typeObjet==POISSON &&etang[partie].objet[y][x].idPoiss==idPoiss){
        etang[partie].objet[y][x].etatPoisson=LIBRE;
    }
    pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
}

void *routine_poisson(void *arg) {
	coord_t *coord = (coord_t *) arg;
    message_t msg;
	int dir, x, y, change,id,unlock=0,partie,ordre;
    int tmpx, tmpy,trouve=0;
    /*tableau int enfuite (-> poisson en fuite) et tab int continue (-> poisson attrapé)*/
	sleep(3);
	y = coord->y;
	x = coord->x;
    partie= coord->partie;
	pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
    id=etang[partie].objet[y][x].idPoiss;
    printf("poiss %d\n",id);
	pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
	srand(time(NULL));
	while (boucle[partie]) { /*s'arrete quand il est tué ou on met une variable -> comment la modifier */
		/*tester s'il y a une ligne proche -> comment faire pour la fuite ?*/
		pthread_mutex_lock(&act[partie]);
		/*attendre signal deplacement cond*/
		while(actPoiss[partie] != BOUGE){
			pthread_cond_wait(&bouge[partie],&act[partie]);
		}
		pthread_mutex_unlock(&act[partie]); /*Il faut peut etre le mettre a la fin*/
		trouve=0;
        unlock=0;
        tmpx=-1;
        tmpy=-1;
        pthread_mutex_lock(&abouges[partie]);
        if(abouge[partie][id]==0){
            pthread_mutex_unlock(&abouges[partie]);
            printf("ici ->regarde %d\n",id);
            unlock=1;
            /* vérifie s'il y a une ligne à proximité*/
            ordre = (rand() % 2);
            if(ordre==0){
                /*on regarde la ligne1 en premier*/
                /* en haut*/
                if(y>0 && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y-1][x].mutObj);
                    if(etang[partie].objet[y-1][x].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y-1][x].gagnantLigne=etang[partie].objet[y-1][x].idLigne1;
                        etang[partie].objet[y-1][x].posLigne=HAUT;
                        trouve=1;
                        tmpx=x;
                        tmpy=y-1;
                    }
                    if(etang[partie].objet[y-1][x].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y-1][x].gagnantLigne=etang[partie].objet[y-1][x].idLigne2;
                        etang[partie].objet[y-1][x].posLigne=HAUT;
                        trouve=1;
                        tmpx=x;
                        tmpy=y-1;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
                }
                /* en bas*/
                if(y<hauteur && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y+1][x].mutObj);
                    if(etang[partie].objet[y+1][x].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y+1][x].gagnantLigne=etang[partie].objet[y+1][x].idLigne1;
                        etang[partie].objet[y+1][x].posLigne=BAS;
                        trouve=1;
                        tmpx=x;
                        tmpy=y+1;
                    }
                    if(etang[partie].objet[y+1][x].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y+1][x].gagnantLigne=etang[partie].objet[y+1][x].idLigne2;
                        etang[partie].objet[y+1][x].posLigne=BAS;
                        trouve=1;
                        tmpx=x;
                        tmpy=y+1;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y+1][x].mutObj);
                }
                /* à droite*/
                if(x<largeur && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y][x+1].mutObj);
                    if(etang[partie].objet[y][x+1].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y][x+1].gagnantLigne=etang[partie].objet[y][x+1].idLigne1;
                        etang[partie].objet[y][x+1].posLigne=DROITE;
                        trouve=1;
                        tmpx=x+1;
                        tmpy=y;
                    }
                    if(etang[partie].objet[y][x+1].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y][x+1].gagnantLigne=etang[partie].objet[y][x+1].idLigne2;
                        etang[partie].objet[y][x+1].posLigne=DROITE;
                        trouve=1;
                        tmpx=x+1;
                        tmpy=y;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x+1].mutObj);
                }
                /* à gauche */
                if(x>0 && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y][x-1].mutObj);
                    if(etang[partie].objet[y][x-1].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y][x-1].gagnantLigne=etang[partie].objet[y][x-1].idLigne1;
                        etang[partie].objet[y][x-1].posLigne=GAUCHE;
                        trouve=1;
                        tmpx=x-1;
                        tmpy=y;
                    }
                    if(etang[partie].objet[y][x-1].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y][x-1].gagnantLigne=etang[partie].objet[y][x-1].idLigne2;
                        etang[partie].objet[y][x-1].posLigne=GAUCHE;
                        trouve=1;
                        tmpx=x-1;
                        tmpy=y;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x-1].mutObj);
                }
                
            }
            else{
                /*on regarde la ligne2 en premier*/
                /* en haut*/
                if(y>0 && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y-1][x].mutObj);
                    if(etang[partie].objet[y-1][x].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y-1][x].gagnantLigne=etang[partie].objet[y-1][x].idLigne2;
                        etang[partie].objet[y-1][x].posLigne=HAUT;
                        trouve=1;
                        tmpx=x;
                        tmpy=y-1;
                    }
                    if(etang[partie].objet[y-1][x].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y-1][x].gagnantLigne=etang[partie].objet[y-1][x].idLigne1;
                        etang[partie].objet[y-1][x].posLigne=HAUT;
                        trouve=1;
                        tmpx=x;
                        tmpy=y-1;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
                }
                /* en bas*/
                if(y<hauteur && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y+1][x].mutObj);
                    if(etang[partie].objet[y+1][x].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y+1][x].gagnantLigne=etang[partie].objet[y+1][x].idLigne2;
                        etang[partie].objet[y+1][x].posLigne=BAS;
                        trouve=1;
                        tmpx=x;
                        tmpy=y+1;
                    }
                    if(etang[partie].objet[y+1][x].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y+1][x].gagnantLigne=etang[partie].objet[y+1][x].idLigne1;
                        etang[partie].objet[y+1][x].posLigne=BAS;
                        trouve=1;
                        tmpx=x;
                        tmpy=y+1;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y+1][x].mutObj);
                }
                /* à droite*/
                if(x<largeur && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y][x+1].mutObj);
                    
                    if(etang[partie].objet[y][x+1].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y][x+1].gagnantLigne=etang[partie].objet[y][x+1].idLigne2;
                        etang[partie].objet[y][x+1].posLigne=DROITE;
                        trouve=1;
                        tmpx=x+1;
                        tmpy=y;
                    }
                    if(etang[partie].objet[y][x+1].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y][x+1].gagnantLigne=etang[partie].objet[y][x+1].idLigne1;
                        etang[partie].objet[y][x+1].posLigne=DROITE;
                        trouve=1;
                        tmpx=x+1;
                        tmpy=y;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x+1].mutObj);
                }
                /* à gauche */
                if(x>0 && !trouve){
                    pthread_mutex_lock(&etang[partie].objet[y][x-1].mutObj);
                    
                    if(etang[partie].objet[y][x-1].idLigne2!=-1 && !trouve){
                        etang[partie].objet[y][x-1].gagnantLigne=etang[partie].objet[y][x-1].idLigne2;
                        etang[partie].objet[y][x-1].posLigne=GAUCHE;
                        trouve=1;
                        tmpx=x-1;
                        tmpy=y;
                    }
                    if(etang[partie].objet[y][x-1].idLigne1!=-1 && !trouve){
                        etang[partie].objet[y][x-1].gagnantLigne=etang[partie].objet[y][x-1].idLigne1;
                        etang[partie].objet[y][x-1].posLigne=GAUCHE;
                        trouve=1;
                        tmpx=x-1;
                        tmpy=y;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x-1].mutObj);
                }
            }
            if(trouve==1){
                pthread_mutex_lock(&abouges[partie]);
                abouge[partie][id]=1;
                pthread_mutex_unlock(&abouges[partie]);
                chrono_poisson(partie,tmpy,tmpx,id);
            }
        }

        if(unlock==0){
            pthread_mutex_lock(&abouges[partie]);
        }

		unlock=0;
		if(abouge[partie][id]==0){
            printf("ici ->bouge %d\n",id);
			pthread_mutex_unlock(&abouges[partie]);
			unlock=1;
			/*sinon se déplacer*/
			dir = (rand() % 4) ;
			change = 0;
			pthread_testcancel();
			switch(dir) {
				case HAUT :
					if(y>0){
						pthread_mutex_lock(&etang[partie].objet[y-1][x].mutObj);
						pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
						if (etang[partie].objet[y-1][x].typeObjet == VIDE) {
                            swap_poiss(&etang[partie].objet[y][x],&etang[partie].objet[y-1][x],1);
							/* envoi de la nouvelle coordonnées
								idPoiss, direction 
							*/
							y--;
							change = 1;
                            msg.typeMessage=M_POISS;
                            msg.idPoisson=id;
                            msg.direction=HAUT;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[partie*2],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            if(write(socketJ[partie*2+1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            pthread_mutex_unlock(&sock[partie]);
						}
						if (change) {
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y+1][x].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang[partie].objet[y-1][x].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
						}
						break;
					}
					
				case DROITE:
					if(x<largeur-1){
						pthread_mutex_lock(&etang[partie].objet[y][x+1].mutObj);
						pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
						if (etang[partie].objet[y][x+1].typeObjet == VIDE) {
                            swap_poiss(&etang[partie].objet[y][x],&etang[partie].objet[y][x+1],1);
						    /* envoi de la nouvelle coordonnées
								idPoiss, direction 
							*/
							x++;
							change = 1;
                            msg.typeMessage=M_POISS;
                            msg.idPoisson=id;
                            msg.direction=DROITE;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[partie*2],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            if(write(socketJ[partie*2+1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            pthread_mutex_unlock(&sock[partie]);
						}
						if (change) {
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y][x-1].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang[partie].objet[y][x+1].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
						}
						break;
					}
				case BAS:
					if(y<hauteur-1){
						pthread_mutex_lock(&etang[partie].objet[y+1][x].mutObj);
						pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
						if (etang[partie].objet[y+1][x].typeObjet == VIDE) {
                            swap_poiss(&etang[partie].objet[y][x],&etang[partie].objet[y+1][x],1);
                            /* envoi de la nouvelle coordonnées
								idPoiss, direction 
							*/
							y++;
							change = 1;
                            msg.typeMessage=M_POISS;
                            msg.idPoisson=id;
                            msg.direction=BAS;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[partie*2],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            if(write(socketJ[partie*2+1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            pthread_mutex_unlock(&sock[partie]);
						}
						if (change) {
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y-1][x].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang[partie].objet[y+1][x].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
						}
						break;
					}
				case GAUCHE:
					if(x!=0){
						pthread_mutex_lock(&etang[partie].objet[y][x-1].mutObj);
						pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
						if (etang[partie].objet[y][x-1].typeObjet == VIDE) {
							swap_poiss(&etang[partie].objet[y][x],&etang[partie].objet[y][x-1],1);
                            /* envoi de la nouvelle coordonnées
								idPoiss, direction 
							*/
							x--;
							change = 1;
                            msg.typeMessage=M_POISS;
                            msg.idPoisson=id;
                            msg.direction=GAUCHE;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[partie*2],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            if(write(socketJ[partie*2+1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors de l'envoi de M_POISS au joueur\n");
                            }
                            pthread_mutex_unlock(&sock[partie]);
						}
						if (change) {
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y][x+1].mutObj);
						}
						else {
							pthread_mutex_unlock(&etang[partie].objet[y][x-1].mutObj);
							pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
						}
						break;
					}
			}

			pthread_mutex_lock(&abouges[partie]);
			abouge[partie][max_poiss]++;
			abouge[partie][id]=1;
			pthread_mutex_lock(&nbPoissons[partie]);
			if(abouge[partie][max_poiss]==nbpoiss[partie]){ /* c'est le dernier poisson qui bouge qui lance l'affichage */
				pthread_mutex_lock(&act[partie]);
				actPoiss[partie]=LIBRE;
				pthread_mutex_unlock(&act[partie]);
			}

			pthread_mutex_unlock(&nbPoissons[partie]);
			pthread_mutex_unlock(&abouges[partie]);
		}
		if(unlock==0){
			pthread_mutex_unlock(&abouges[partie]);
		}
        printf("il s'est passé un truc %d\n",id);
		/* c'est pas comme ça qu'on va afficher une fois par tour
			-> bloquer threadPoiss au bout de 1 tour et afficher ?
					si oui, comment ?*/
	}
	return NULL;
}

void simulation_initialiser(int partie) {
	int i, j;
	
    threads_poissons[partie]=malloc(max_poiss*sizeof(pthread_t *));
	for (i = 0; i < max_poiss; i++){		/* Au depart il n'y a aucun poisson dans la simulation */
        threads_poissons[partie][i] = malloc(sizeof(pthread_t));
	}

    etang[partie].objet=malloc(hauteur*sizeof(objet_t *));
    for(j=0;j<hauteur;j++){
        etang[partie].objet[j]=malloc(largeur*sizeof(objet_t));
    }
	for (i = 0; i < hauteur; i++) {	/* Initialisation des cases de la simulation */
		for (j = 0; j < largeur; j++) {
			/* etang[partie].objet[i][j].threadPoisson = 0; */
			etang[partie].objet[i][j].idLigne1 = -1;
            etang[partie].objet[i][j].idLigne2 = -1;
			etang[partie].objet[i][j].typeObjet = VIDE;
			pthread_mutex_init(&etang[partie].objet[i][j].mutObj, NULL);
		}
	}
	
}

void simulation_stopper(int partie) {
	int i;
	
	pthread_mutex_lock(&nbPoissons[partie]);
	for(i=0;i<nbpoiss[partie];i++){
		pthread_cancel(*threads_poissons[partie][i]);
		free(threads_poissons[partie][i]);
	}
	pthread_mutex_unlock(&nbPoissons[partie]);
	/* pthread_cancel(gerant[partie]); */
	free(threads_poissons[partie]);
	free(abouge[partie]);
	for(i=0;i<hauteur;i++){
		free(etang[partie].objet[i]);
	}
	free(etang[partie].objet);
}
/* faire un stop partie */
void stopPartie(int partie){
    void * res;
    
    boucle[partie]=0;
    
    if(close(socketJ[partie*2]) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
    if(close(socketJ[partie*2+1]) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    pthread_join(threadTCP[partie],&res);
    pthread_join(jTCP[partie],&res);
    /*stop simul pour MAX_PARTIE*/
}

void stopServeur(int sig){
    void * res;
    int j;

    if(sig== SIGINT){

        for(j=0;j<MAX_PARTIE;j++){
            boucle[j]=0;
        }
        supprimerSemaphores(semid);
        if(close(sockfdUDP) == -1) {
            perror("Erreur lors de la fermeture de la socket ");
            exit(EXIT_FAILURE);
        }
        for(j=0;j<MAX_PARTIE;j++){
            pthread_join(threadTCP[j],&res);
            stopPartie(j);
        }
        for(j=0;j<MAX_JOUEURS;j++){
            pthread_join(jTCP[j],&res);
        }
        bouclePrincipale =0;
        /*stop simul pour MAX_PARTIE*/
    }
}




void * joueurTCP(void * args){ /*va communiquer avec un joueur*/
    int* paraThread;
    int idJ,retour,partie,x,y;
    int tmpx,tmpy;
    message_t msg;

    paraThread= (int*)args; /* [0] -> socket du joueur, [1] -> id du joueur */
    idJ=paraThread[1];
    socketJ[idJ]=paraThread[0];
    partie=paraThread[2];
    
    /*
    if(write(socket,&msg,sizeof(message_t))==-1){
        perror("Erreur lors de l'envoi de WINGAME au joueur\n");
        exit(EXIT_FAILURE);
    }
    */
    printf("les messages\n");
    while(boucle[partie]){
        tmpx=-1;
        tmpy=-1;
        if((retour=read(socketJ[idJ], &msg, sizeof(message_t))) == -1) {
            if(errno !=EINTR && errno !=EAGAIN){
                perror("Erreur lors de la lecture de la taille du message joueur ");
                exit(EXIT_FAILURE);
            }
        }
        switch(msg.typeMessage){
            
            case (P_LIGNE):
                x=msg.position[1];
                y=msg.position[0];
                if(x!=-1 && y!=-1){
                    pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
                    if(idJ==0){
                        etang[partie].objet[y][x].idLigne1=idJ;
                    }
                    else{
                        etang[partie].objet[y][x].idLigne2=idJ;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
                    msg.typeMessage=OKO_LIGNE;
                    pthread_mutex_lock(&sock[partie]);
                    if(write(socketJ[idJ],&msg,sizeof(message_t))==-1){
                        perror("Erreur lors envoi ok ligne");
                    }
                    pthread_mutex_unlock(&sock[partie]);
                }
            break;

            case (R_LIGNE):
                x=msg.position[1];
                y=msg.position[0];
                if(x!=-1 && y!=-1){
                    pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
                    if(etang[partie].objet[y][x].gagnantLigne==idJ){
                        
                        if(etang[partie].objet[y][x].posLigne==HAUT){
                            tmpy=y+1;
                            tmpx=x;
                        }
                        if(etang[partie].objet[y][x].posLigne==BAS){
                            tmpy=y-1;
                            tmpx=x;
                        }
                        if(etang[partie].objet[y][x].posLigne==DROITE){
                            tmpy=y;
                            tmpx=x-1;
                        }
                        if(etang[partie].objet[y][x].posLigne==GAUCHE){
                            tmpy=y;
                            tmpx=x+1;
                        }
                        
                        if(etang[partie].objet[tmpy][tmpx].etatPoisson==MORDU){
                            /* poisson attrapé à temps !! 
                            -> on tue le poisson,on cancel le thread poiss*/
                            pthread_mutex_lock(&etang[partie].objet[tmpy][tmpx].mutObj);
                            pthread_mutex_lock(&nbPoissons[partie]);
                            nbpoiss[partie]--;
                            pthread_mutex_lock(&nbPoissons[partie]);
                            msg.typeMessage=PRISE;
                            msg.typeObjet=etang[partie].objet[tmpy][tmpx].typeObjet;
                            if(etang[partie].objet[tmpy][tmpx].typeObjet==POISSON){
                                msg.typePoisson=etang[partie].objet[tmpy][tmpx].typePoisson;  
                            }
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[idJ],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors envoi ok ligne");
                            }
                            pthread_mutex_unlock(&sock[partie]);
                            pthread_mutex_unlock(&etang[partie].objet[tmpy][tmpx].mutObj);
                        }
                        else{
                            msg.typeMessage=PRISE;
                            msg.typeObjet=VIDE;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[idJ],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors envoi ok ligne");
                            }
                            pthread_mutex_unlock(&sock[partie]);
                        }
                        etang[partie].objet[y][x].gagnantLigne=-1;
                    }
                    else if(etang[partie].objet[y][x].typeObjet==PNEU){
                        
                        msg.typeMessage=PRISE;
                        msg.typeObjet=PNEU;
                        pthread_mutex_lock(&sock[partie]);
                        if(write(socketJ[idJ],&msg,sizeof(message_t))==-1){
                            perror("Erreur lors envoi ok ligne");
                        }
                        pthread_mutex_unlock(&sock[partie]);
                        etang[partie].objet[y][x].typeObjet=VIDE;
                    }
                    pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
                }
            break;

            

            /* le joueur a gagné */
            case (WIN_GAME): /*envoi aux deux joueurs ENDGAME
                arret jTCP des deux joueurs et du threadTCP de la partie */
                msg.typeMessage=ENDGAME;
                pthread_mutex_lock(&sock[partie]);
                if(write(socketJ[idJ],&msg,sizeof(message_t))==-1){
                    perror("Erreur lors envoi ok ligne");
                }
                if(idJ==0){
                    if(write(socketJ[idJ+1],&msg,sizeof(message_t))==-1){
                        perror("Erreur lors envoi ok ligne");
                    }
                }
                else{
                    if(write(socketJ[idJ-1],&msg,sizeof(message_t))==-1){
                        perror("Erreur lors envoi ok ligne");
                    } 
                }
                pthread_mutex_unlock(&sock[partie]);
            
            break;
            case (PIEGE):
                switch(msg.typeObjet){
                    /* mode furtif activé */
                    case (FURTIF_ON): 
                        sleep(1); /* XXX on verra */
                    break;

                    /* mode furtif arrêté*/
                    case (FURTIF_OFF):
                        sleep(1); /* XXX on verra */
                    break;

                    case REQUIN:
                        /*creer requin */
                        x=msg.position[1];
                        y=msg.position[0];
                        pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
                        if(etang[partie].objet[y][x].typeObjet==VIDE){
                            pthread_mutex_lock(&nbPoissons[partie]);
                            if(nbpoiss[partie]<max_poiss){
                                etang[partie].objet[y][x].idJoueur=idJ;
                                creerRequin(y,x,partie,idJ);
                            }
                            pthread_mutex_unlock(&nbPoissons[partie]);
                        }
                        else{
                            /*envoi msg pas ok*/
                            msg.typeMessage=PIEGE;
                            msg.position[1]=x;
                            msg.position[0]=y;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[idJ-1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors envoi ok ligne");
                            }
                            pthread_mutex_unlock(&sock[partie]);
                        }
                        pthread_mutex_unlock(&etang[partie].objet[y][x].mutObj);
                    
                    break;
                    case PNEU:
                        x=msg.position[1];
                        y=msg.position[0];
                        pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
                        if(etang[partie].objet[y][x].typeObjet==VIDE){
                            etang[partie].objet[y][x].idJoueur=idJ;    
                            etang[partie].objet[y][x].typeObjet=PNEU;
                            /*envoi msg pas ok*/
                            msg.typeMessage=PIEGE;
                            msg.typeObjet=PNEU;
                            msg.direction=1;
                            msg.idJoueur=idJ;
                            msg.position[1]=x;
                            msg.position[0]=y;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[idJ-1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors envoi ok ligne");
                            }
                            pthread_mutex_unlock(&sock[partie]);
                        }
                        else{
                            /*envoi msg pas ok*/
                            msg.typeMessage=PIEGE;
                            msg.typeObjet=PNEU;
                            msg.direction=0;
                            msg.position[1]=x;
                            msg.position[0]=y;
                            pthread_mutex_lock(&sock[partie]);
                            if(write(socketJ[idJ-1],&msg,sizeof(message_t))==-1){
                                perror("Erreur lors envoi ok ligne");
                            }
                            pthread_mutex_unlock(&sock[partie]);
                        }
                        pthread_mutex_lock(&etang[partie].objet[y][x].mutObj);
                    break;
                }
            break;
        }
    
    }
    free(paraThread);
}

void* pthreadTCP(void* args) {
    coord_t * coord;
	int* paraThread;
    int statut;
    void * res;
    struct sockaddr_in adresseTCP;
    int j,i,idPartie;
    int sockClient [2]={0,0};
    message_t msg;
    int paraA [3], paraB [3];
    paraThread= (int*)args; /* [0] -> adresse TCP, [1] -> id de la partie */
    idPartie = paraThread[1];
    /* Création de la socket TCP */
    if((fdTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }
    /* Création de l'adresse du serveur */
    memset(&adresseTCP, 0, sizeof(struct sockaddr_in));
    adresseTCP.sin_family = AF_INET;
    adresseTCP.sin_addr.s_addr = htonl(INADDR_ANY);
    adresseTCP.sin_port = htons(paraThread[0]);

    /* Nommage de la socket */
    if(bind(fdTCP, (struct sockaddr*)&adresseTCP, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors du nommage de la socket ici");
        exit(EXIT_FAILURE);
    }

    /* Mise en mode passif de la socket */
    if(listen(fdTCP, 1) == -1) {
        perror("Erreur lors de la mise en mode passif ");
        exit(EXIT_FAILURE);
    }
    Vas(idPartie,semid);
    j=0;
    while(sockClient[0]==0 || sockClient[1]==0){/* && remplacé par || pour reception des 2 msg */
        /* Attente d'une connexion */
        printf("Serveur : attente de connexion...\n");
        if((sockClient[j] = accept(fdTCP, NULL, NULL)) == -1) {
            perror("Erreur lors de la demande de connexion ");
            exit(EXIT_FAILURE);
        }
        if(read(sockClient[j], &msg, sizeof(message_t) ) == -1) {
            perror("Erreur lors de la lecture de la taille du message socket ");
            exit(EXIT_FAILURE);
        }
        if(msg.typeMessage==CO_TCP_CS){
            printf("TCP réussi\n");
        }
        printf("Serveur TCP: message recu.\n");
        j++;
    }
/* lancement d'une partie*/
    msg.typeMessage = GAME;
    msg.nbPoissons=max_poiss;
    msg.largeur= largeur;
    msg.hauteur= hauteur;
    /* création de l'étang */
    
    /* init de l'étang */
    simulation_initialiser(idPartie);
    
    /* envoi des informations GAME : id joueur, hauteur, largeur, grille */
    for(j=0;j<2;j++){
        msg.idJoueur=j;
        printf("J'envoie au joueur %d\n",msg.idJoueur);
        if(write(sockClient[j], &msg , sizeof(message_t)) == -1) {
            perror("Erreur lors de l'envoi des infos GAME au joueur\n");
            exit(EXIT_FAILURE);
        }
    }
    
    /*debut du jeu*/
    statut = pthread_create(&gerant[idPartie], NULL, GestionAction,(void *)&idPartie);

    paraA[0]=sockClient[0];
    paraA[1]=0;
    paraA[2]=idPartie;
    printf("creation thread %d\n", idPartie*2+0);
    statut= pthread_create(&jTCP[idPartie*2], NULL, joueurTCP,paraA);
    if(statut!=0){
        printf("Pb création thread\n");
    }
    paraB[0]=sockClient[1];
    paraB[1]=1;
    paraA[2]=idPartie;
    printf("creation thread %d\n", idPartie*2+1);
    statut= pthread_create(&jTCP[idPartie*2+1], NULL, joueurTCP,paraB);
    if(statut!=0){
        printf("Pb création thread\n");
    }
    /* création de 20% de  MAX pour débuter */

    for(i=0;i<max_poiss/5;i++){
		coord=(coord_t*)malloc(sizeof(coord_t)); 
		coord->partie=idPartie;
		creerPoisson(coord);
		statut=pthread_create(threads_poissons[idPartie][i], NULL,routine_poisson,(void *)coord);
		if(statut!=0){
			printf("Pb création threadpoiss %d\n",i);
		}
	}

    printf("poissons %d\n",nbpoiss[idPartie]);

    pthread_join(jTCP[idPartie*2],&res);

    pthread_join(jTCP[idPartie*2+1],&res);
    pthread_join(gerant[idPartie],&res);
    /* XXX cancel aussi peut être */
    /* join gerant et joueurTCP */
    free(paraThread);
    return EXIT_SUCCESS;
}





int main (int argc, char * argv []){

/*déclarations*/
    struct sigaction action;

    int numPort=1,pair=0,port;
    socklen_t taille;
    struct sockaddr_in adresseServeurUDP,adresseServeurTCP;
    message_t reqUDP,repUDP;
    int statut;
    int paraThread [2];
    void* res;
    unsigned short val[MAX_PARTIE];/*tableau d'initialisation des sem*/
    int i,j;

    int nombreJoueurs=0,j1=0,j2=0,trouve=0,nombrePartie=-1;
    reponseUDP_t adresseClientUDP [MAX_JOUEURS];

/*vérification des arguments
    adresse IP
    numéro de port UDP sur lequel il va attendre les clients
    dimension de l'étang (largeur + hauteur)
*/

    if(argc != 4) {
        fprintf(stderr, "Usage : %s  port dimensions (L | H)\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " port  : port d'écoute du serveur\n");
        fprintf(stderr, " dimensions : largeur et hauteur de la zone de jeu\n");
        exit(EXIT_FAILURE);
    }
    largeur = atoi(argv[2]);
    hauteur = atoi(argv[3]);
    max_poiss= (largeur*hauteur)/5;

    for(j=0;j<MAX_JOUEURS;j++){
        adresseClientUDP[j].vide=0;
        memset(&adresseClientUDP[j].adr, 0, sizeof(struct sockaddr_in));
    }

    action.sa_handler = stopServeur;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if(sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Erreur lors du positionnement\n");
        exit(EXIT_FAILURE);
    }
    
    
    for(i=0;i<MAX_PARTIE;i++){
        /* INIT des MUTEX*/
        pthread_mutex_init(&nbPoissons[i],NULL);
        pthread_mutex_init(&abouges[i],NULL);
        pthread_mutex_init(&act[i],NULL);
        pthread_mutex_init(&tour[i],NULL);
        pthread_mutex_init(&sock[i],NULL);
        /* INIT des COND*/
        pthread_cond_init(&bouge[i],NULL);
        pthread_cond_init(&pose[i],NULL);
        pthread_cond_init(&peche[i],NULL);
        /* INIT des variables d'actions*/
        actPoiss[i]=BOUGE;
        boucle[i]=1;
        nbpoiss[i]=0;
    }
    for(j=0;j<MAX_PARTIE;j++){
        abouge[j]=malloc(sizeof(int)*(max_poiss+1));
        for(i=0;i<=max_poiss;i++){
            abouge[j][i]=0;
        }
    }
    
/*Création tableau sémaphore pour la connexion*/

    semid = creerSemaphores(CLE_SEM);

    /* Initialisation du tab d'initialisation des sem*/
    for (i=0; i<MAX_PARTIE; i++) {
        val[i] = 1;
    }

    /* Initialisation des sémaphores */
    if(semctl(semid, 0, SETALL, val) == -1) {
        fprintf(stderr, "Erreur lors de l'initialisation des sémaphores\n");
        exit(EXIT_FAILURE);
    }

/*attente des clients en UDP*/

        /* Création de la socket UDP */

    if((sockfdUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

        /* Création de l'adresse du serveur UDP */
    memset(&adresseServeurUDP, 0, sizeof(struct sockaddr_in));
    adresseServeurUDP.sin_family = AF_INET;
    adresseServeurUDP.sin_port = htons(atoi(argv[1]));
    adresseServeurUDP.sin_addr.s_addr = htonl(INADDR_ANY);

        /* Nommage de la socket UDP */
    if(bind(sockfdUDP, (struct sockaddr*)&adresseServeurUDP, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors du nommage de la socket ");
        exit(EXIT_FAILURE);
    }
    
    taille=sizeof(struct sockaddr_in);
        /* Tant que le serveur est en route*/
        /* Attente des clients */
    while(bouclePrincipale){
        /* Attente de la requête du client UDP */
        printf("Serveur en attente du message du client.\n");
        if(nombreJoueurs < MAX_JOUEURS){
            if(recvfrom(sockfdUDP, &reqUDP, sizeof(message_t), 0,(struct sockaddr*)&adresseClientUDP[nombreJoueurs].adr, &taille) == -1) {
                perror("Erreur lors de la réception du message ");
                exit(EXIT_FAILURE);
            }
            printf("Serveur : requete UDP reçue.\n");
            if(reqUDP.typeMessage != CO_UDP_CS){
                printf("Problème dans la réception\n Pas le bon type de message\n");
                printf("Requete igonrée\n");
            }
            else{
                adresseClientUDP[nombreJoueurs].vide=1;
                nombreJoueurs++;
                printf("nb joueurs %d\n",nombreJoueurs);
                if(nombreJoueurs > 0 && nombreJoueurs%2==0){
                    /*On associe deux joueurs ensemble*/
                    j1=0;
                    j2=-1;
                    trouve =0;
                    while(!trouve && (j1<MAX_JOUEURS && j2 < MAX_JOUEURS)){
                        if(adresseClientUDP[j1].vide == 1){
                            if(j2 != -1){
                                trouve =1;
                            }
                            else{
                                j2=j1;
                                j1++;
                            }
                        }
                        else{
                            j1++;
                        }
                    }
                    nombrePartie++;
                    /* Envoi du message UDP */
                    repUDP.typeMessage = INFO_TCP_SC;
                    
                    /* Création de l'adresse TCP du serveur */
                    memset(&adresseServeurTCP, 0, sizeof(struct sockaddr_in));
                    adresseServeurTCP.sin_family = AF_INET;
                    adresseServeurTCP.sin_addr.s_addr = htonl(INADDR_ANY);
                    port =atoi(argv[1])+numPort;
                    adresseServeurTCP.sin_port = htons(port);
                    
                    repUDP.adresse = adresseServeurTCP;
                    repUDP.idPartie= nombrePartie;
                    if(sendto(sockfdUDP, &repUDP, sizeof(message_t), 0, (struct sockaddr*)&adresseClientUDP[j1].adr, sizeof(adresseClientUDP[j1].adr)) == -1) {
                        perror("Erreur lors de l'envoi du message j1");
                        exit(EXIT_FAILURE);
                    } 
                    if(sendto(sockfdUDP, &repUDP, sizeof(message_t), 0, (struct sockaddr*)&adresseClientUDP[j2].adr, sizeof(adresseClientUDP[j2].adr)) == -1) {
                        perror("Erreur lors de l'envoi du message j2");
                        exit(EXIT_FAILURE);
                    } 

                    /*On supprime les deux adresses j1 et j2*/
                    adresseClientUDP[j1].vide=0;
                    adresseClientUDP[j2].vide=0;
                    memset(&adresseClientUDP[j1].adr, 0, sizeof(struct sockaddr_in));
                    memset(&adresseClientUDP[j2].adr, 0, sizeof(struct sockaddr_in));

                    /*création d'un thread avec le numéro de port de adresseServeurTCP à cet instant*/   
                    paraThread[0]=port;
                    paraThread[1]=numPort-1;
                    statut= pthread_create(&threadTCP[nombreJoueurs/2-1], NULL, pthreadTCP,(void *)&paraThread);
                    if(statut!=0){
                        printf("Pb création thread\n");
                        exit(EXIT_FAILURE);
                    }
                    if(pair){
                        numPort ++;
                        pair =0;
                    }
                    else{
                        pair =1;
                    }
                }
            }
        }
        
    }
    
/*envoi aux client des informations TCP*/

/*création de la grille et début partie*/

/*partie*/
    /*pose de ligne ->chrono*/
    /*retour de la ligne*/

    /*mode furtif*/

    /*déplacement poisson ->avertir les clients*/

/*fin quand réception gagné ou déconnexion*/ 
    for(j=0;j<MAX_PARTIE;j++){
        pthread_join(threadTCP[j],&res);
    }
    for(j=0;j<MAX_JOUEURS;j++){
        pthread_join(jTCP[j],&res);
    }
    /* Fermeture de la socket */
    if(close(sockfdUDP) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
        
    return EXIT_SUCCESS;
}