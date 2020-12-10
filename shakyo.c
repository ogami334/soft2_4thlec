#include <stdio.h>

void test (int *a){
	printf("%d\n", (*a)++);
	test(a);
}

int main(void){
	int a = 0;
	test(&a);
	return 0;
}