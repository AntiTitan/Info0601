#include "sauvegarde.h"

int main(int argc, char * argv []){
    unsigned char mat [NB_L][NB_C];
    int fichier,check,i,j;
    unsigned char chaine [NB_C*NB_L+MAX_NOM+FLOC];
    int floc[FLOC];

    if(argc==2){

        fichier=ouvrir_fichier(argv[1]);
        if(fichier==-1){
            perror("Error open: ");
            return EXIT_FAILURE;
        }

        check=lire_fichier_s(fichier,chaine,argv[1]);
        if(check == -1){
        perror("Error read: ");
        return EXIT_FAILURE;
        }

        ecrire_matrice(mat,chaine,argv[1]);
        for(i=0;i<NB_L;i++){
            for(j=0;j<NB_C;j++){
                printf("%c ",mat[i][j]);
            }
            printf("\n");
        }

        ecrire_flocon(floc,chaine,argv[1]);
        printf("\n");

        check=fermer_fichier(fichier);
        if(check==1){
            perror("Error close: ");
            return EXIT_FAILURE;
        }

    }
    else{
        fprintf(stderr,"Nombre d'arguments incorrect !!!!\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}