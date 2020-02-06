#include <time.h>
#include "TP1.h"

int** updateFlocon(int** mat, int* nbf) {
    
}
int ajouterFlocon(int** mat){
    int nb;
    nb = rand() % 30 + 1;
    mat[0][nb]=1;
    return nb;
}

void initZoneAlea(int** mat) {
    int i,j,nb;
    srand(time(NULL));
    for(i=0;i<15;i++) {
        for(j=0;j<30;j++) {
            nb = rand() % 10 + 1;
            if(nb==1){
                mat[i][j]=2;
            }
        }
    }
    for(i=0;i<30;i++){
        if(mat[0][i]==2){ mat[0][i]=0;}
    }
}

int main(){
    /* Matrice indique la présence de flocons (1) et d'obstacles (2) */
    int matrice [15][30];
    int i,j, nbf=0, ajout;
    int cpt=0,col;

    /*Initialisation de la matrice*/
    initZoneAlea(matrice);
    printf("init\n");
    for(i=0; i<15; i++) {
        for(j=0;j<30;j++){
            printf("%d ",matrice[i][j]);
        }
        printf("\n");
    }
    printf("nbf : %d\n",nbf);
    col=ajouterFlocon(matrice);
    col=ajouterFlocon(matrice);
    matrice = updateFlocon(matrice, &nbf);
    printf("update 1\n");
    for(i=0; i<15; i++) {
        for(j=0;j<30;j++){
            printf("%d ",matrice[i][j]);
        }
        printf("\n");
    } 
    printf("nbf : %d\n",nbf);
    col=ajouterFlocon(matrice);
    col=ajouterFlocon(matrice);
    matrice = updateFlocon(matrice, &nbf);
    printf("update 2\n");
    for(i=0; i<15; i++) {
        for(j=0;j<30;j++){
            printf("%d ",matrice[i][j]);
        }
        printf("\n");
    } 
    printf("nbf : %d\n",nbf);
    col=ajouterFlocon(matrice);
    col=ajouterFlocon(matrice);
    matrice = updateFlocon(matrice, &nbf);       
    printf("update 3\n");
    for(i=0; i<15; i++) {
        for(j=0;j<30;j++){
            printf("%d ",matrice[i][j]);
        }
        printf("\n");
    } 
    printf("nbf : %d\n",nbf);
    matrice = updateFlocon(matrice, &nbf);       
    printf("update 4\n");
    for(i=0; i<15; i++) {
        for(j=0;j<30;j++){
            printf("%d ",matrice[i][j]);
        }
        printf("\n");
    } 
    printf("nbf : %d\n",nbf);


    return EXIT_FAILURE;
}