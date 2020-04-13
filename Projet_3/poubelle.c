

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



/*

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
    
    wrefresh(fen_obj);
    