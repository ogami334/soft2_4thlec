#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *fp;
	if ( (fp = fopen(argv[1],"rb")) == NULL){
		// fopen は失敗した場合 errno の変数をセットする
		// perror は errno の番号に対応したメッセージを返してくれる
		perror(argv[1]); 
		return EXIT_FAILURE;
	}
	// 100バイトのchar配列（スタック領域）
	
	// 1バイトを100個分読み出す。rsize には読み出した個数が格納される
    size_t listsize;
    fread(&listsize,sizeof(size_t),1,fp);
    double *list = (double *) malloc(sizeof(double) * listsize);
	size_t rsize = fread(list, sizeof(double), listsize, fp); 
	
	printf("%zu Byte read\n", rsize);
    for (int i=0;i<1000;i++) {
        printf("%lf\n",list[i]);
    }
	
	// 文字列だと解釈したい場合は終端記号\0を追加
	/*buf[rsize] = '\0';
	printf("%s",buf);*/
	
	return EXIT_SUCCESS;
}