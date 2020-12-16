#include <stdio.h>
#include <stdlib.h>

typedef struct matrix{
    int a;
    int b;
    int c;
    int d;
} Matrix;

Matrix mul(Matrix,Matrix);
Matrix powan(int);

int main(int argc,char **argv) {
    int n =atoi(argv[1]);//あとで変える
    int fibon =powan(n-1).a + powan(n-1).b;
    printf("%d\n",fibon);
    return 0;
}

Matrix mul(Matrix A,Matrix B) {
    Matrix C ={ .a = A.a * B.a + A.b * B.c , 
                .b = A.a * B.b + A.b * B.d ,
                .c = A.c * B.a + A.d * B.c ,
                .d = A.c * B.b + A.d * B.d
             };
    return C;
}

Matrix powan(int n) {
    Matrix A = { .a =1, .b =1, .c =1, .d =0};
    if (n==1) {
        return A;
    }
    else if (n%2 ==0) {
        return mul(powan(n/2),powan(n/2));
    }
    else {
        return mul(mul(powan((n-1)/2), powan((n-1)/2)), A);
    }
}
//割とすぐオーバーフローしてしまいます