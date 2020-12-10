#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *fp;
	if ( (fp = fopen(argv[1],"r")) == NULL){
		// fopen は失敗した場合 errno の変数をセットする
		// perror は errno の番号に対応したメッセージを返してくれる
		perror(argv[1]); 
		return EXIT_FAILURE;
	}
	// 100バイトのchar配列（スタック領域）
	char buf[100];
	
	// 1バイトを100個分読み出す。rsize には読み出した個数が格納される
    int size;
    fscanf(fp,"%d\n",&size);
    double *list =malloc(sizeof(double)*size);
    for (int i=0;i<size;i++) {
        fscanf(fp,"%lf\n",list+i);
    }
	free(list);
	return EXIT_SUCCESS;
}