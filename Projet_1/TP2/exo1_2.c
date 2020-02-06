#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char* argv [] ){
    int ret,taille,len,t,tt,size;
    char chaine [200], chaine2 [200];

    if(argc !=1){
        ret = open(argv[1],O_RDWR | O_CREAT, 00777);
        if(ret!= -1){
            do {
                printf("Entrez la chaîne de cara à écrire (200 max):\n");
                if(scanf("%s",chaine)!=1){
                    perror("Error scanf: ");
                    return EXIT_FAILURE;
                }
            } while(strlen(chaine)>=200);

            taille =lseek(ret, 0, SEEK_END);
            if(taille ==-1){
                perror("Error lseek: ");
                return EXIT_FAILURE;
            }

            /*Ecriture*/
            len = strlen(chaine) + 1;
            if(write(ret,&len,sizeof(len))==-1){
                perror("Error write: ");
                return EXIT_FAILURE;
            }

            if(write(ret,chaine,len*sizeof(char))==-1){
                perror("Error write2: ");
                return EXIT_FAILURE;
            }

            /*Remise de la tete de lecture au debut*/
            taille =lseek(ret, 0, SEEK_SET);
            if(taille ==-1){
                perror("Error lseek2: ");
                return EXIT_FAILURE;
            }

            printf("\nAffichage du contenu du fichier\n\n");

            /*Lecture*/
            do {
                t = read(ret, &size,sizeof(int));
                if(t==-1){
                    perror("Error read: ");
                    return EXIT_FAILURE;
                }
                tt=read(ret,chaine2,size*sizeof(char));
                if(tt==-1){
                    perror("Error read2: ");
                    return EXIT_FAILURE;
                } else if (tt != 0){
                    printf("%s\n", chaine2);
                }
            } while(tt != 0);

            /*Fermeture*/
            if(close(ret) == -1){
                perror("Error close: ");
                return EXIT_FAILURE;
            }
        }
        else{
            perror("Error open: ");
            return EXIT_FAILURE;
        }
    }
    else{
        fprintf(stderr,"Nombre d'arguments incorrect !!!!\n");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}