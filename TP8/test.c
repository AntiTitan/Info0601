#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <stdio.h>      /* Pour printf, perror */
#include <curses.h>

int main(){
    int sors =0,i=100000;
    while(!sors){
        i=getch();
        printf("%d",i);
        if(i==KEY_F(2)){
            sors=1;
        }
        if(i==-1){
            printf("zolala\n");
            sors=1;
        }
    }
    return EXIT_SUCCESS;
}