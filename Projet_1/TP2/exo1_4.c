#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char* argv [] ){
    int fic1,fic2,taille,len,rd,cpt;
    char chaine [200];

    if(argc !=2){
        fic1 = open(argv[1],O_RDWR | O_CREAT, 00777);
        fic2 = open(argv[2],O_RDWR | O_CREAT | O_EXCL, 00777);
        if(fic2==-1){
            if (errno == EEXIST) {
                perror("Error fic2: ");
                return EXIT_FAILURE;
            } else{
                perror("Error open fic2: ");
                return EXIT_FAILURE;
            }
        } else{
            if(fic1!= -1){
                cpt=0;
                do {
                    /*Deplacement de la tete de lecture*/
                    taille =lseek(fic1, cpt, SEEK_SET);
                    if(taille ==-1){
                        perror("Error lseek: ");
                        return EXIT_FAILURE;
                    }

                    /*Lecture*/
                    rd = read(fic1, chaine,sizeof(char)*50);
                    if(rd==-1){
                        perror("Error read: ");
                        return EXIT_FAILURE;
                    }
                    
                    /*Ecriture*/
                    else if(rd != 0){    len = strlen(chaine) + 1;
                        if(write(fic2,chaine,len*sizeof(char))==-1){
                            perror("Error write: ");
                            return EXIT_FAILURE;
                        }
                    }
                    cpt+=len;
                } while(rd != 0);

                /*Fermeture*/
                if(close(fic1) == -1){
                    perror("Error close fic1: ");
                    return EXIT_FAILURE;
                }
                if(close(fic2) == -1){
                    perror("Error close fic2: ");
                    return EXIT_FAILURE;
                }
            }
            else{
                perror("Error open fic1: ");
                return EXIT_FAILURE;
            }
        }
    }
    else{
        fprintf(stderr,"Nombre d'arguments incorrect !!!!\n");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}