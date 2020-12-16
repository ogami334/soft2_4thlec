#include <stdio.h>
#define n 5

int main(int argc,char **argv) {
    int item_number =n;
    double value[n] ={5.0,7.0,10.0,4.0,4.0};
    double weight[n] ={6.0,8.0,11.0,4.0,3.0};
    FILE *fp;
    if ((fp =fopen(argv[1],"wb")) == NULL) {
        printf("Error");
    }
    fwrite(&item_number,sizeof(int),1,fp);
    fwrite(value,sizeof(double),n,fp);
    fwrite(weight,sizeof(double),n,fp);
    fclose(fp);

}