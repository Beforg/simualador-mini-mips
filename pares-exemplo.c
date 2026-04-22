#include <stdio.h>

int main(void){
    int i = 0;
    int n = 10;
    int sum = 0;

    printf("Imprimindo nros pares:\n");
    for(i = 0; i < n;i++){
        sum+=2;
        printf("[%d]\n",sum);
    }

    return 0;
}
