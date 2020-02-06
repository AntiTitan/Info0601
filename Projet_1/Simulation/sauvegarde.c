#include "sauvegarde.h"


int ouvrir_fichier(char* nomfich){
    return(open(nomfich,O_RDWR | O_CREAT,00777));
}
int lire_fichier_d(int fic,unsigned char chaine [],char * nom){
    return(read(fic,chaine,sizeof(unsigned char)*(NB_C*NB_L+strlen(nom))));

}
int lire_fichier_s(int fic,unsigned char chaine [],char * nom){
    return(read(fic,chaine,sizeof(unsigned char)*(NB_C*NB_L+strlen(nom))+FLOC*sizeof(int)));
}
int lire_flocon(int fic,unsigned char chaine [],char * nom){
    return(read(fic,chaine,sizeof(unsigned char)*(NB_C*NB_L+strlen(nom))+FLOC*sizeof(int)));
}
int afficher_fichier(int fic){
    int taille,retour,size;
    char chaine [NB_C];
    do {
        taille = read(fic, &size,sizeof(int));
        if(taille==-1){
            perror("Error read size: ");
            return EXIT_FAILURE;
        }
        retour=read(fic,chaine,size*sizeof(char));
        if(retour==-1){
            perror("Error read file: ");
            return EXIT_FAILURE;
        } else if (retour != 0){
            printf("%s\n", chaine);
        }
    } while(retour != 0);
    return 1;
}
int ecrire_fichier_s(int fic,unsigned char mat [][NB_C],int floc [FLOC],char * nom){
    int tete,i;
    int size =strlen(nom);
    unsigned char f;
    tete =lseek(fic, 0, SEEK_SET);
    if(tete ==-1){
        perror("Error lseek: ");
        return -1;
    }
    if(write(fic,nom,size*sizeof(char))==-1){
        perror("Error write: ");
        return EXIT_FAILURE;
    }
    for(i=0;i<NB_L;i++) {
        if(write(fic,mat[i],NB_C*sizeof(unsigned char))==-1){
            perror("Error write: ");
            return EXIT_FAILURE;
        }
    }
  
    if(write(fic,floc,FLOC*sizeof(int))==-1){
        perror("Error write: ");
        return EXIT_FAILURE;
    }
   
    if(write(fic,&f,FLOC*sizeof(unsigned char))==-1){
        perror("Error write: ");
        return EXIT_FAILURE;
    }
    return 1;
}
int ecrire_fichier_d(int fic,unsigned char mat [][NB_C],char * nom){
    int len=NB_C,tete,i;
    int size =strlen(nom);
    tete =lseek(fic, 0, SEEK_SET);
    if(tete ==-1){
        perror("Error lseek: ");
        return -1;
    }

    if(write(fic,nom,size*sizeof(unsigned char))==-1){
        perror("Error write: ");
        return EXIT_FAILURE;
    }

    for(i=0;i<NB_L;i++) {
        if(write(fic,mat[i],len*sizeof(unsigned char))==-1){
            perror("Error write: ");
            return EXIT_FAILURE;
        }
    }
    return 1;
}
int fermer_fichier(int fic){
    return(close(fic) == -1);
}
void ecrire_matrice(unsigned char mat [][NB_C],unsigned char ch [],char * nom){
    int i,j;
    int taille=strlen(nom);
    for(i=0;i<NB_L;i++){
        for(j=0;j<NB_C;j++){
            mat[i][j]=ch[taille+(i*NB_C)+j];
        }
    }
}
void ecrire_flocon(int flocon [FLOC],unsigned char ch [],char * nom){
    int j, taille;
    char c[2]={0};
    taille=strlen(nom);
    
    for(j=0;j<FLOC;j++){
        c[0]=ch[taille+NB_L*NB_C+j];
        flocon[j]=atoi(c);
        printf("%u\n",ch[taille+NB_L*NB_C+j]); 
    }
    printf("position flocon %d | %d ", flocon[0],flocon[1]);

}