#define _XOPEN_SOURCE
/*include*/

#include "fonctions_sys.h"
#include "struct_message.h"

int boucle=1;
int semid;
int sockfdUDP;
int fdTCP;
int largeur, hauteur;

pthread_t threadTCP[MAX_PARTIE];

void stopServeur(int sig){
    void * res;
    int j;

    if(sig== SIGINT){
        boucle=0;
        supprimerSemaphores(semid);
        if(close(sockfdUDP) == -1) {
            perror("Erreur lors de la fermeture de la socket ");
            exit(EXIT_FAILURE);
        }
        for(j=0;j<MAX_PARTIE;j++){
            pthread_join(threadTCP[j],&res);
        }
    }
}

void* pthreadTCP(void* args) {

	int* paraThread;
    struct sockaddr_in adresseTCP;
    int j;
    int sockClient [2]={0,0};
    message_t msg;
    paraThread= (int*)args;
    
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
        perror("Erreur lors du nommage de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Mise en mode passif de la socket */
    if(listen(fdTCP, 1) == -1) {
        perror("Erreur lors de la mise en mode passif ");
        exit(EXIT_FAILURE);
    }
    Vas(paraThread[1],semid);
    j=0;
    while(sockClient[0]==0 || sockClient[1]==0){/* && remplacé par || pour reception des 2 msg */
        /* Attente d'une connexion */
        printf("Serveur : attente de connexion...\n");
        if((sockClient[j] = accept(fdTCP, NULL, NULL)) == -1) {
            perror("Erreur lors de la demande de connexion ");
            exit(EXIT_FAILURE);
        }
        if(read(sockClient[j], &msg, sizeof(message_t) ) == -1) {
            perror("Erreur lors de la lecture de la taille du message ");
            exit(EXIT_FAILURE);
        }
        if(msg.typeMessage==CO_TCP_CS){
            printf("TCP réussi\n");
        }
        printf("Serveur TCP: message recu.\n");
        j++;
    }
/* lancement d'une partie*/
    /* création de la grille */


    /* envoi des informations */
    msg.typeMessage = GAME;
    msg.grille.largeur= largeur;
    msg.grille.hauteur= hauteur;
    if(write(sockClient[j], &msg , sizeof(message_t)) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
    }
    
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
    largeur ++;hauteur++;

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
    while(boucle){
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
                    printf("Serveur : message envoyé à j1.\n"); 
                    if(sendto(sockfdUDP, &repUDP, sizeof(message_t), 0, (struct sockaddr*)&adresseClientUDP[j2].adr, sizeof(adresseClientUDP[j2].adr)) == -1) {
                        perror("Erreur lors de l'envoi du message j2");
                        exit(EXIT_FAILURE);
                    }
                    printf("Serveur : message envoyé à j2.\n"); 

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
    for(j=0;j<MAX_PARTIE;j++){
        pthread_join(threadTCP[j],&res);
    }
/*envoi aux client des informations TCP*/

/*création de la grille et début partie*/

/*partie*/
    /*pose de ligne ->chrono*/
    /*retour de la ligne*/

    /*mode furtif*/

    /*déplacement poisson ->avertir les clients*/

/*fin quand réception gagné ou déconnexion*/  
    /* Fermeture de la socket */
    if(close(sockfdUDP) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
        
    return EXIT_SUCCESS;
}