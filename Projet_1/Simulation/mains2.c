#include "sauvegarde.h"

int main(int argc, char * argv []){
    unsigned char mat [NB_L][NB_C];
    int fichier,check,i,j;
    int tete;
    int flocon [2];

    flocon[0]=5; flocon[1]=9;

    for(i=0;i<NB_L;i++){
        for(j=0;j<NB_C;j++){
            if(i==j){
                mat[i][j]='2';
            }
            else{
                mat[i][j]='0';
            }
        }
    }
    

    if(argc==2){

        fichier=ouvrir_fichier(argv[1]);
        if(fichier==-1){
            perror("Error open: ");
            return EXIT_FAILURE;
        }
        
        tete =lseek(fichier, 0, SEEK_SET);
        if(tete ==-1){
            perror("Error lseek: ");
            return -1;
        }
        
        check=ecrire_fichier_s(fichier,mat,flocon,argv[1]);
        if(check == -1){
            perror("Error write: ");
            return EXIT_FAILURE;
        }

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