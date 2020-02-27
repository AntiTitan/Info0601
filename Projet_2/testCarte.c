#include "fcontroleur.h"

int stopControleur=0;
int arret[3];

void handler_Controleur(int signal){
    pid_t pid;
    if(signal == SIGINT){
        printf("recu controller\nnum pid ? ");
        if(scanf("%d",&pid)==-1){
            exit(EXIT_FAILURE);
        }
        kill(pid,SIGINT);
        /*suppression outils IPC */
        supprimerFile(arret[0]);
        supprimerMemoire(arret[1]);
        supprimerSemaphores(arret[2]);
        stopControleur =1;
    }
}

int main (int argc, char * argv []){
    int i,j,fic;
    info_t *map;

/*Verification des arguments
    nom fichier carte
    nb max de voitures
    clé file messages
    clé seg memoire partagée
    cle tableau sémaphores
*/

    if (argc != 2) {
        fprintf(stderr, "Nombre d'arguments incorrect:  ./controleur ficCartte \nficCarte = lasvegas.bin | reims.bin | vide.bin\n");
        exit(EXIT_FAILURE);
    }
    if ((map=malloc(sizeof(info_t)))==-1){
        fprintf(stderr, "Erreur lors de l'allocation memoire ");
        exit(EXIT_FAILURE);
    }
/*Remplissage seg memoire*/
    /*Extraction info du fichier*/
    if((fic = ouvrir_fichier(argv[1]))==-1){
        fprintf(stderr, "Erreur lors de l'ouverture du fichier carte ");
        exit(EXIT_FAILURE);
    }
    if(lire_fichier(fic, map->carte.grille, argv[1])==-1){
        fprintf(stderr, "Erreur lors de la lecture du fichier carte ");
        exit(EXIT_FAILURE);
    }
    /*Attachement du segment memoire partagée*/
    printf("carte %s",argv[1]);
    for(i=0;i<NB_L;i++){
        for(j=0;j<NB_C;j++){
            printf("%d ",map->carte.grille[i][j]);
        }
        printf("\n");
    }


    return EXIT_SUCCESS;
}