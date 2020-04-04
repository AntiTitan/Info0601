

/*tester s'il y a une ligne proche -> comment faire pour la fuite ?*/
		pthread_mutex_lock(&etang.objet[y][x].mutObj);
		if(!enfuite[id]){
			/* en haut */
			pthread_mutex_lock(&etang.objet[y-1][x].mutObj);
			if(etang.objet[y-1][x].typeObjet == 5){
				/*attrapé par etang.objet[y-1][x].idJoueur */
				/* blocage ?*/
			}
			pthread_mutex_unlock(&etang.objet[y-1][x].mutObj);

			/* en bas */
			pthread_mutex_lock(&etang.objet[y+1][x].mutObj);
			if(etang.objet[y+1][x].typeObjet == 5){
				/*attrapé par etang.objet[y+1][x].idJoueur */
				/* blocage ?*/
			}
			pthread_mutex_unlock(&etang.objet[y+1][x].mutObj);

			/* à droite */
			pthread_mutex_lock(&etang.objet[y][x+1].mutObj);
			if(etang.objet[y][x+1].typeObjet == 5){
				/*attrapé par etang.objet[y][x+1].idJoueur */
				/* blocage ?*/
			}
			pthread_mutex_unlock(&etang.objet[y][x+1].mutObj);

			/* à gauche */
			pthread_mutex_lock(&etang.objet[y][x-1].mutObj);
			if(etang.objet[y][x-1].typeObjet == 5){
				/*attrapé par etang.objet[y][x-1].idJoueur */
				/* blocage ?*/
			}
			pthread_mutex_unlock(&etang.objet[y][x-1].mutObj);
		}