#include <stdio.h>
int main(int argc, char **argv) {
    FILE *fp;
    fp =fopen("calc_magicnumber.dat","wb");
    char *s ="Hello,World\n";
    size_t size =fwrite(s,sizeof(char),15,fp);
    fclose(fp);
    fp =fopen("calc_magicnumber.dat","rb");
    float buf[100] ={0};
    size_t read =fread(buf,sizeof(char),15,fp);
    for (int i=0;i<4;i++) {
        printf("%.150f\n",(float)buf[i]);
    }
    fclose(fp);
}