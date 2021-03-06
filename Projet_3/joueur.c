#define _XOPEN_SOURCE
/*include*/
#include <stdlib.h>      /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>       /* Pour printf, fprintf, perror */
#include <sys/socket.h>  /* Pour socket, bind */
#include <arpa/inet.h>   /* Pour sockaddr_in */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */
#include "struct_message.h"
#include "fonctions_sys.h"



int actJoueur,boucle=1,etatPartie=0,nbpoiss,max_poiss, *abouge;
int sockfdTCP,sockfdUDP;
ligne_t ligne;
grille_t etang;
joueur_t joueur;
poisson_t * poiss;

pthread_t affiche;
pthread_t gerant;
pthread_t clic;

pthread_mutex_t sock =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mess =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t obj =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t abouges =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t act =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t etat =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t poissons =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutJ =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutpoiss =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutLigne =PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t aff = PTHREAD_COND_INITIALIZER;

WINDOW *fen_sim,*fen_msg,*fen_obj;
WINDOW *fen_box_sim,*fen_box_msg,*fen_box_obj;

void simulation_stopper() {
	int i;

    boucle =0;
	for(i=0;i<etang.hauteur;i++){
		free(etang.objet[i]);
	}
	free(etang.objet);

    free(poiss);
    free(abouge);

	ncurses_stopper();

    delwin(fen_msg);
    delwin(fen_sim);
    delwin(fen_box_msg);
    delwin(fen_box_sim);
    /* Fermeture de la socket TCP */
    if(close(sockfdTCP) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Fermeture de la socket UDP */
    if(close(sockfdUDP) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
}
void stopJoueur(int sig){
    void * res;
    if(sig== SIGINT){
        simulation_stopper();
        pthread_join(affiche,&res);
        pthread_join(gerant,&res);
    }
}

void* GestionAction(void* arg) {
    message_t msg;
    while(1) {
        pthread_mutex_lock(&act);
        /* variable bloquée */

        if (actJoueur == AFFICHE) {
            pthread_cond_broadcast(&aff);
        }

        /* variable débloquée */
        pthread_mutex_unlock(&act);

        pthread_mutex_lock(&mutJ);
        if(joueur.nbPoints==15){
            /*arrêt clic et envoi message */
            pthread_mutex_lock(&etat);
            etatPartie=WIN_GAME;

            msg.typeMessage=WIN_GAME;
            pthread_mutex_lock(&sock);
            if(write(sockfdTCP, &msg , sizeof(message_t)) == -1) {
                perror("Erreur lors de l'envoi du message ");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&sock);

            pthread_mutex_unlock(&etat);
        }
        pthread_mutex_unlock(&mutJ);

        pthread_mutex_lock(&etat);
        if(etatPartie == ENDGAME){
            /* déclanchement fin du jeu*/
            simulation_stopper();
        }
        pthread_mutex_unlock(&etat);
        sleep(1);
    }

    return NULL;
}

void* ClicAction(void* arg){
    int ch,item_actif = VIDE,poseObj;
    message_t msg;
    MEVENT event;
    ncurses_souris(); 
	
	while(boucle){ 
		/*gère clic pour la ligne ( et des objets )*/
        pthread_mutex_lock(&act);
        if(actJoueur==LIBRE){
            ch = getch();
            poseObj=0;
            switch(ch) {
                case KEY_MOUSE :
                    if (getmouse(&event) == OK) {
                        /*dans l'écran*/
                        if (event.y <=etang.hauteur+1 && event.y >= 1 && event.x >= 1 && event.x <= etang.largeur+1) {
                            /*un objet est-il en cours ? -> on le pose*/
                            pthread_mutex_lock(&mutLigne);
                            if(item_actif!=VIDE){ 
                                /*la case est-elle vide ?*/
                                pthread_mutex_lock(&etang.objet[event.y][event.x].mutObj);
                                pthread_mutex_lock(&mutJ);
                                if(item_actif==PNEU && joueur.poireaus>=150){
                                    poseObj=1;
                                }
                                if(item_actif==DYNAMITE && joueur.poireaus>=200){
                                    poseObj=1;
                                }
                                if(item_actif==REQUIN && joueur.poireaus>=300){
                                    poseObj=1;
                                }
                                if(item_actif==FURTIF_ON && joueur.poireaus>=500){
                                    poseObj=1;
                                }

                                pthread_mutex_unlock(&mutJ);
                                if(etang.objet[event.y][event.x].typeObjet==VIDE && poseObj){
                                    /*envoi au serveur */
                                    msg.typeMessage=PIEGE;
                                    msg.idJoueur=joueur.idJoueur;
                                    msg.typeObjet=item_actif;
                                    msg.position[0]=event.y;
                                    msg.position[1]=event.x;
                                    pthread_mutex_lock(&sock);
                                    if(write(sockfdTCP, &msg , sizeof(message_t)) == -1) {
                                        perror("Erreur lors de l'envoi du message ");
                                        exit(EXIT_FAILURE);
                                    }
                                    pthread_mutex_unlock(&sock);
                                    etang.objet[event.y][event.x].typeObjet=item_actif;
                                    pthread_mutex_lock(&mutJ);
                                    if(item_actif==PNEU ){
                                        joueur.poireaus-=150;
                                    }
                                    if(item_actif==DYNAMITE ){
                                        joueur.poireaus-=200;
                                    }
                                    if(item_actif==REQUIN ){
                                        joueur.poireaus-=300;
                                    }
                                    if(item_actif==FURTIF_ON ){
                                        joueur.poireaus-=500;
                                    }
                                    pthread_mutex_unlock(&mutJ);
                                    item_actif=VIDE;
                                }
                                pthread_mutex_unlock(&etang.objet[event.y][event.x].mutObj);
                                
                            }
                            /*la ligne est relevée ? -> on la pose*/
                            else if(ligne.pos==RELEVE){
                                ligne.y=event.y;
                                ligne.x=event.x;
                                msg.typeMessage=P_LIGNE;
                                msg.position[0]=event.y;
                                msg.position[1]=event.x;
                                /* XXX indiquer id J ?*/
                                pthread_mutex_lock(&sock);
                                if(write(sockfdTCP, &msg , sizeof(message_t)) == -1) {
                                    perror("Erreur lors de l'envoi du message ");
                                    exit(EXIT_FAILURE);
                                }
                                pthread_mutex_unlock(&sock);
                                /* XXX envoi pos ligne*/
                            }
                            /*la ligne est posée ? -> on la relève*/
                            else if(ligne.pos==POSE){
                                /* XXX envoi releve*/
                                msg.typeMessage=R_LIGNE;
                                msg.position[0]=ligne.y;
                                msg.position[1]=ligne.x;
                                /* XXX indiquer id J ?*/
                                pthread_mutex_lock(&sock);
                                if(write(sockfdTCP, &msg , sizeof(message_t)) == -1) {
                                    perror("Erreur lors de l'envoi du message ");
                                    exit(EXIT_FAILURE);
                                }
                                pthread_mutex_unlock(&sock);
                            }
                            pthread_mutex_unlock(&mutLigne);
                        }
                        /*dans les objets*/
                        else if (event.y >= HAUTEUR_MSG+3 && event.y <= (2+HAUTEUR_MSG)*2-1 && event.x >=etang.largeur+3 && event.x <=etang.largeur+3+LARGEUR_MSG ) {
                            if (event.y >= HAUTEUR_MSG+4 && event.y <= HAUTEUR_MSG+10 && event.x >=etang.largeur+4 && event.x <=etang.largeur+17 ){
                                /*pneu*/
                                item_actif=PNEU;
                            }
                            else if (event.y >= HAUTEUR_MSG+11 && event.y <= HAUTEUR_MSG+17 && event.x >=etang.largeur+4 && event.x <=etang.largeur+17 ){
                                /*requin*/
                                item_actif=REQUIN;
                            }
                            else if (event.y >= HAUTEUR_MSG+4 && event.y <= HAUTEUR_MSG+10 && event.x >=etang.largeur+19 && event.x <=etang.largeur+32 ){
                                /*dynamite*/
                                item_actif=DYNAMITE;
                            }
                            else if (event.y >= HAUTEUR_MSG+11 && event.y <= HAUTEUR_MSG+17 && event.x >=etang.largeur+19 && event.x <=etang.largeur+32 ){
                                /*furtif*/
                                item_actif=FURTIF_ON;
                            }
                        }
                    }
                break;
            }
        }
        pthread_mutex_unlock(&act);
	}
    return NULL;
}

void simulation_initialiser() {
	int i, j;
    boucle=1;
	for (i = 0; i < etang.hauteur; i++) {	/* Initialisation des cases de la simulation */
		for (j = 0; j < etang.largeur; j++) {
			etang.objet[i][j].typeObjet = VIDE;
			pthread_mutex_init(&etang.objet[i][j].mutObj, NULL);
		}
	}
	
}

void * affichage(void * arg){
	int i,j;
	while(boucle){
		/* on fait affichage quand tous les poissons ont bougé ou qd un poisson est crée */
		pthread_mutex_lock(&act);

		while(actJoueur != AFFICHE){
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
							wattron(fen_sim,COLOR_PAIR(0));
							mvwprintw(fen_sim, i, j, " "); /* fond noir */
							wattroff(fen_sim,COLOR_PAIR(0));
						}
						else{
							mvwprintw(fen_sim, i, j, " "); /* fond bleu */
						}
						/*fond noir ou bleu selon idJ*/
						break;
				}
                if(etang.objet[i][j].idLigne1!=-1){
                    if(etang.objet[i][j].idLigne1==joueur.idJoueur){
                        wattron(fen_sim,COLOR_PAIR(5));
						mvwprintw(fen_sim, i, j, " "); /* fond  violet */
						wattron(fen_sim,COLOR_PAIR(5));
                    }
                    else{
    					mvwprintw(fen_sim, i, j, " "); /* fond bleu */
                    
                    }
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
		actJoueur=LIBRE;
		pthread_mutex_unlock(&act);
	}
	return NULL;
}

int main (int argc, char * argv []){
/*déclarations*/
    void * res;
    int i,j=0,statut;
    struct sockaddr_in adresseServeurUDP , adresseServeurTCP;
    message_t reqUDP,repUDP;
    message_t reqTCP,repTCP;
    struct sigaction action;
    int numPartie,semid;
    int idTemp,x,y,trouve;

/*vérification des arguments
    adresse IP
    numéro de port UDP du serveur
*/

   if(argc != 3) {
        fprintf(stderr, "Usage : %s adresseIP port \n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, " adresseIP : adresse IP du serveur du serveur\n");
        fprintf(stderr, " port  : port d'écoute du serveur\n");
        exit(EXIT_FAILURE);
    }
    reqUDP.typeMessage = CO_UDP_CS;
    semid = recupererSemaphores(CLE_SEM);
    action.sa_handler = stopJoueur;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if(sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Erreur lors du positionnement\n");
        exit(EXIT_FAILURE);
    }
 /*connexion au serveur en UDP*/

        /* Création de la socket UDP */
    if((sockfdUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }
        /* Création de l'adresse du serveur */
    memset(&adresseServeurUDP, 0, sizeof(struct sockaddr_in));
    adresseServeurUDP.sin_family = AF_INET;
    adresseServeurUDP.sin_port = htons(atoi(argv[2]));
    if(inet_pton(AF_INET, argv[1], &adresseServeurUDP.sin_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }
        /* Envoi du message UDP */
    if(sendto(sockfdUDP, &reqUDP, sizeof(message_t), 0, (struct sockaddr*)&adresseServeurUDP, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
    }

 /*reception des informations TCP*/

        /* Attente de la reponse du serveur UDP */
    if(recvfrom(sockfdUDP, &repUDP, sizeof(message_t), 0, NULL, NULL) == -1) {
      perror("Erreur lors de la réception du message ");
      exit(EXIT_FAILURE);
    }
    adresseServeurTCP = repUDP.adresse;
    numPartie= repUDP.idPartie;
        /* Création de la socket TCP */
    if((sockfdTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

    if(inet_pton(AF_INET, argv[1], &adresseServeurTCP.sin_addr.s_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }
    
 /*connexion au serveur en TCP*/
    Peux(numPartie,semid);
        /* Connexion au serveur */
    if(connect(sockfdTCP, (struct sockaddr*)&adresseServeurTCP, sizeof(adresseServeurTCP)) == -1) {
        perror("Erreur lors de la connexion ");
        exit(EXIT_FAILURE);
    }
    reqTCP.typeMessage=CO_TCP_CS;

    if(write(sockfdTCP, &reqTCP , sizeof(message_t)) == -1) {
      perror("Erreur lors de l'envoi du message ");
      exit(EXIT_FAILURE);
    }

    /*reception des données du jeu */
    if(read(sockfdTCP, &repTCP, sizeof(message_t)) == -1) {
        if(errno != EINTR){
            perror("Erreur lors de la lecture de la taille du message sock");
            exit(EXIT_FAILURE);
        }
    }
    
    /*reception des données de la partie -> joueur et grille*/
    joueur.idJoueur=repTCP.idJoueur;
    max_poiss=repTCP.nbPoissons;

    poiss=malloc(sizeof(poisson_t)*max_poiss);
    abouge=malloc(sizeof(int)*(max_poiss+1));
    for(i=0;i<max_poiss;i++){
        poiss[i].id=-1;
        abouge[i]=0;
    }
    abouge[max_poiss]=0;
    /*création locale de la grille*/
    etang.largeur=repTCP.largeur;
    etang.hauteur=repTCP.hauteur;

    etang.objet=malloc(sizeof(objet_t *)*etang.hauteur);
    for(i=0;i<etang.hauteur;i++){
        etang.objet[i]=malloc(sizeof(objet_t)*etang.largeur);
        for(j=0;j<etang.largeur;j++){
            etang.objet[i][j].typeObjet=VIDE;
            etang.objet[i][j].idLigne1=-1;
        }
    }

    ligne.y=-1;
    ligne.x=-1;
    ligne.pos=RELEVE;
    simulation_initialiser();
	ncurses_initialiser();
	ncurses_couleurs();
    /* Definition de la palette */
    init_pair(0, COLOR_WHITE, COLOR_BLACK); /* pour les pneus et pneu */
    init_pair(1, COLOR_WHITE, COLOR_BLACK); /* pour les box */
    init_pair(2, COLOR_WHITE, COLOR_BLUE); /* pour l'etang */
    init_pair(3, COLOR_BLACK, COLOR_YELLOW); /* pour les poissons */
    init_pair(4, COLOR_BLACK, COLOR_GREEN); /* pour les requins */
	init_pair(5, COLOR_WHITE, COLOR_MAGENTA); /* pour les lignes */
    init_pair(6, COLOR_BLACK, COLOR_RED); /* pour les dynamites */
    
    /*
    creer_fenetre_box_sim(fen_box_sim,etang.hauteur,etang.largeur);
	creer_fenetre_sim(fen_sim,fen_box_sim,etang.hauteur,etang.largeur);
	creer_fenetre_box_msg(fen_box_msg,etang.largeur);
	creer_fenetre_msg(fen_msg,fen_box_msg,etang.largeur);
    creer_fenetre_box_obj(fen_box_obj,etang.largeur);
    pthread_mutex_lock(&obj);
	creer_fenetre_obj(fen_obj,fen_box_obj,etang.hauteur,etang.largeur);
    pthread_mutex_unlock(&obj);
    */

    fen_box_sim = newwin(etang.hauteur + 2, etang.largeur + 2, 0, 0);
	box(fen_box_sim, 0, 0);
	wbkgd(fen_box_sim, COLOR_PAIR(1));
	mvwprintw(fen_box_sim, 0, 3, "SIMULATION");	
	wrefresh(fen_box_sim);

    fen_sim = subwin(fen_box_sim,etang.hauteur, etang.largeur, 1, 1);
	/* Colore le fond de la fenêtre */
    wbkgd(fen_sim, COLOR_PAIR(2));
    wrefresh(fen_sim);

    fen_box_msg = newwin(HAUTEUR_MSG + 2, LARGEUR_MSG + 2, 0, etang.largeur + 2);
	box(fen_box_msg, 0, 0);
	wbkgd(fen_box_msg, COLOR_PAIR(1));
	mvwprintw(fen_box_msg, 0, (etang.largeur + 2) / 2 - 4, "MESSAGES");
	wrefresh(fen_box_msg);

    fen_msg = subwin(fen_box_msg, HAUTEUR_MSG,LARGEUR_MSG, 1, etang.largeur + 3);
	scrollok(fen_msg, TRUE);
	wbkgd(fen_msg, COLOR_PAIR(5));
    wrefresh(fen_msg);

    fen_box_obj = newwin(HAUTEUR_MSG + 2, LARGEUR_MSG + 2, HAUTEUR_MSG + 2, etang.largeur + 2);
	box(fen_box_obj, 0, 0);
	wbkgd(fen_box_obj, COLOR_PAIR(1));
	mvwprintw(fen_box_obj, 0, (etang.largeur + 2) / 2 - 4, "OBJETS");
	wrefresh(fen_box_obj);

    fen_obj = subwin(fen_box_obj, HAUTEUR_MSG,LARGEUR_MSG, HAUTEUR_MSG + 3, etang.largeur + 3);
	/*scrollok(fen_obj, TRUE);*/
	wbkgd(fen_obj, COLOR_PAIR(1));
    /*definition des zones*/
    pthread_mutex_lock(&obj);
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
    pthread_mutex_unlock(&obj);
    wrefresh(fen_obj);

    pthread_mutex_lock(&obj);
    mvwprintw(fen_msg,0,0,"fen créées");
    wrefresh(fen_msg);
    pthread_mutex_unlock(&obj);

    statut = pthread_create(&gerant, NULL, GestionAction, NULL);
	if(statut!=0){
        printf("problème\n");
        exit(EXIT_FAILURE);
    }
    statut = pthread_create(&affiche, NULL,affichage,NULL); 
    if(statut!=0){
        printf("problème\n");
        exit(EXIT_FAILURE);
    }
    statut = pthread_create(&clic, NULL,ClicAction,NULL); 
    if(statut!=0){
        printf("problème\n");
        exit(EXIT_FAILURE);
    }
	actJoueur=LIBRE; /* XXX */

    /*début de partie -> tant que pas fin du jeu (ou deconnexion -> à gérer)*/
    while(boucle){
        /*attente de messages du serveur -> thread affichage*/
        mvwprintw(fen_msg,0,0,"en attende de msg");
        wrefresh(fen_msg);
        pthread_mutex_lock(&sock);
        if(read(sockfdTCP, &repTCP, sizeof(message_t)) == -1) {
            if(errno != EINTR){
                perror("Erreur lors de la lecture de la taille du message ici");
                exit(EXIT_FAILURE);
            }
        } 
        pthread_mutex_unlock(&sock); 
        switch(repTCP.typeMessage){
            /*resultat pêche */
            case (PRISE): 
                switch(repTCP.typeObjet){
                    case(POISSON):
                        pthread_mutex_lock(&mutJ);
                        switch(repTCP.typePoisson){
                            case(1):
                            joueur.poireaus+=100;
                            break;
                            case(2):
                            joueur.poireaus+=200;
                            break;
                            case(3):
                            joueur.poireaus+=500;
                            break;
                        }
                        joueur.nbPoints+=1;
                        pthread_mutex_unlock(&mutJ);
                    break;
                    case(PNEU):
                        sleep(3);
                    break;
                    case(DYNAMITE):
                        sleep(3);
                    break;
                    case(REQUIN):
                        pthread_mutex_lock(&mutJ);
                        joueur.nbPoints-=1;
                        joueur.poireaus-=100;
                        pthread_mutex_unlock(&mutJ);
                    break;
                }
                
            break;

            /*creation poiss (id,x,y,type) -> changement nbPoiss et ajout dans poiss[] */
            case (C_POISS):
                pthread_mutex_lock(&mutpoiss);
                pthread_mutex_lock(&poissons);
                poiss[nbpoiss].id=repTCP.idPoisson;
                poiss[nbpoiss].x=repTCP.position[1];
                poiss[nbpoiss].y=repTCP.position[0];
                poiss[nbpoiss].type=repTCP.typePoisson;
                nbpoiss++;
                pthread_mutex_unlock(&poissons);
                pthread_mutex_unlock(&mutpoiss);
            break;

            /*mouvements poiss -> les poiss
                chaque poiss -> idPoiss + nouvelle position
                on accède au poisson id (ancienne pos) -> accès à la case ancienne pos
                changement de etang et poisson
            */
            case (M_POISS): 
                pthread_mutex_lock(&mutpoiss);
                pthread_mutex_lock(&poissons);
                idTemp=repTCP.idPoisson;
                trouve=0;
                j=0;
                while(!trouve && j<nbpoiss){
                    if(poiss[j].id==repTCP.idPoisson){
                        trouve=1;
                    }
                    else{
                        j++;
                    }
                }
                y=poiss[j].y;
                x=poiss[j].x;
                pthread_mutex_lock(&etang.objet[y][x].mutObj);
                switch(repTCP.direction){
                    
                    case HAUT :
                        pthread_mutex_lock(&etang.objet[y-1][x].mutObj);
                        swap_poiss(&etang.objet[y][x],&etang.objet[y-1][x],0);
                        pthread_mutex_unlock(&etang.objet[y-1][x].mutObj);
                        
                    break;
                    case BAS:
                        pthread_mutex_lock(&etang.objet[y+1][x].mutObj);
                        swap_poiss(&etang.objet[y][x],&etang.objet[y+1][x],0);
                        pthread_mutex_unlock(&etang.objet[y+1][x].mutObj);
                    break;
                    case DROITE:
                        pthread_mutex_lock(&etang.objet[y][x+1].mutObj);
                        swap_poiss(&etang.objet[y][x],&etang.objet[y][x+1],0);
                        pthread_mutex_unlock(&etang.objet[y][x+1].mutObj);
                    break;
                    case GAUCHE:
                        pthread_mutex_lock(&etang.objet[y][x-1].mutObj);
                        swap_poiss(&etang.objet[y][x],&etang.objet[y][x-1],0);
                        pthread_mutex_unlock(&etang.objet[y][x-1].mutObj);
                    break;
                }
                pthread_mutex_lock(&abouges);
                abouge[idTemp]=1;
                abouge[max_poiss]++;
                if(abouge[max_poiss]==nbpoiss){
                    actJoueur=AFFICHE;
                }
                pthread_mutex_unlock(&abouges);
                pthread_mutex_unlock(&etang.objet[y][x].mutObj);
                pthread_mutex_unlock(&poissons);
                pthread_mutex_unlock(&mutpoiss);
            break;

            /*fin poisson (id)*/
            case (F_POISS): 
                pthread_mutex_lock(&mutpoiss);
                idTemp=repTCP.idPoisson;
                trouve=0;
                while(!trouve && j<nbpoiss){
                    if(poiss[j].id==repTCP.idPoisson){
                        trouve=1;
                    }
                    else{
                        j++;
                    }
                }
                y=poiss[j].y;
                x=poiss[j].x;
                poiss[j].id=-1;
                poiss[j].type=0;
                poiss[j].y=-1;
                poiss[j].x=-1;
                pthread_mutex_lock(&etang.objet[y][x].mutObj);
                kill_poiss(&etang.objet[y][x] , 0);
                pthread_mutex_unlock(&etang.objet[y][x].mutObj);
                pthread_mutex_unlock(&mutpoiss);
            break;

            /*fin du jeu */
            case (ENDGAME): 
                pthread_mutex_lock(&etat);
                etatPartie = ENDGAME;
                pthread_mutex_unlock(&etat);
            break;
            /*resultat pose de ligne */
            case (OKO_LIGNE): 
                pthread_mutex_lock(&mutLigne);
                x=ligne.x;
                y=ligne.y;
                pthread_mutex_lock(&etang.objet[y][x].mutObj);
                etang.objet[y][x].idLigne1=joueur.idJoueur;
                pthread_mutex_lock(&etang.objet[y][x].mutObj);
                ligne.pos=POSE;
                pthread_mutex_unlock(&mutLigne);
            break;
            case (PIEGE):
                x=repTCP.position[1];
                y=repTCP.position[0];
                switch(repTCP.typeObjet){
                    case PNEU:
                        if(repTCP.direction == 1){
                            pthread_mutex_lock(&etang.objet[y][x].mutObj);
                            etang.objet[y][x].typeObjet=PNEU;
                            etang.objet[y][x].idJoueur=repTCP.idJoueur;
                            pthread_mutex_lock(&etang.objet[y][x].mutObj);   
                        }
                    break;
                }
            break;   
        }
    }

    pthread_join(affiche,&res);
    pthread_join(gerant,&res);
    
    return (EXIT_SUCCESS);
}