
/*tiré de poiss.c */
void * affichage(void * arg){
	int i,j;
	while(1){

		for(i=0;i<etang.hauteur;i++){
			for(j=0;j<etang.largeur;j++){
				/* on bloque tous les mutex un par un ? -> j'ai peur que des trucs déja affichés soient modifiés */
				/* ou si on met un mutex sur l'etang entier, ça bloque tout d'un coup ?*/
				pthread_mutex_lock(&etang.objet[i][j].mutObj);
				switch(etang.objet[i][j].typeObjet){
					case(POISSON) :
						switch(etang.objet[i][j].idPoiss){
							case(POISSON1) :
								/* fond jaune avec 1 en noir */
							break;
							case(POISSON2) :
								/* fond jaune avec 2 en noir */
							break;
							case(POISSON3) :
								/* fond jaune avec 3 en noir */
							break;
						}
					break;
					case(REQUIN) :
						/*tester idJoueur du requin*/
						/* fond jaune avec idPoiss en noir 
							ou
						   fond vert*/
					break;
					case(VIDE) :

					break;
					case(DYNAMITE) :
						/* je ne sais pas s'il y a besoin,
						 ça explose direct ou pas ?*/
					break;
					case(PNEU) :
						/*tester idJoueur du pneu*/
						/*fond noir ou bleu selon idJ*/
					break;
					case(LIGNE) :
						/*tester idJoueur de la ligne*/
						/*fond gris ou bleu selon idJ*/
					break;
				}
				pthread_mutex_unlock(&etang.objet[i][j].mutObj);
			}
		}
	}
}