#include <stdio.h>
#include <stdlib.h>

int main(void){
  float f[4];

  // 以下に適切な値を設定する
  f[0] = 1143139122437582505939828736.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000;
  f[1] = 66592994030331831990133719040.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000;
  f[2] = 0.000000000000000000000000000000010998196979361949578541716812575563205984430515569654396475229862772948763449676334857940673828125000000000000000000000;
  f[3] = 0.000000000000000000000000000000000000009040774117865994194283477237266551383174441333548019322834338531991171805657359072938561439514160156250000000000;

  // "Hello,World\n" と出力してほしい
  printf("%s",(char*)f);
  return 0;
}
//OSはMac,コンパイラはgccです.
