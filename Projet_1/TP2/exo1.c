#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char* argv [] ){
    int ret,taille;

    if(argc !=1){
        ret = open(argv[1],O_RDONLY);
        if(ret!= -1){
            printf("Le fichier existe !\n");
            taille =lseek(ret, 0, SEEK_END);
            if(taille ==-1){
                perror("Error: ");
                return EXIT_FAILURE;
            }
            printf("Le fichier a pour taille %d octets\n", taille);

            if(close(ret) == -1){
                perror("Error: ");
                return EXIT_FAILURE;
            }
        }
        else{
            perror("Error: ");
            return EXIT_FAILURE;
        }
    }
    else{
        fprintf(stderr,"Nombre d'arguments incorrect !!!!\n");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}