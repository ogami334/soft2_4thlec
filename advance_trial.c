#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#define inf 1000000

// 町の構造体（今回は2次元座標）を定義
typedef struct
{
  int x;
  int y;
} City;

// 描画用
typedef struct
{
  int width;
  int height;
  char **dot;
} Map;

// 整数最大値をとる関数
int max(const int a, const int b)
{
  return (a > b) ? a : b;
}
double min (const double a,const double b) {
    return (a < b) ? a : b;
}

// プロトタイプ宣言
// draw_line: 町の間を線で結ぶ
// draw_route: routeでの巡回順を元に移動経路を線で結ぶ
// plot_cities: 描画する
// distance: 2地点間の距離を計算
// solve(): TSPをといて距離を返す/ 引数route に巡回順を格納

void draw_line(Map map, City a, City b);
void draw_route(Map map, City *city, int n, const int *route);
void plot_cities(FILE* fp, Map map, City *city, int n, const int *route);
double distance(City a, City b);
double solve(const City *city, int n, int *route, int *visited);
double solvedp(const City *city, int n,int *opt_route);
double dfs(int state,int node,const City *city,double **dp,int n);
Map init_map(const int width, const int height);
void free_map_dot(Map m);
City *load_cities(const char* filename,int *n);
int *best_route(double **dp,const int n,const City *city,int *opt_route);

Map init_map(const int width, const int height)
{
  char **dot = (char**) malloc(width * sizeof(char*));
  char *tmp = (char*)malloc(width*height*sizeof(char));
  for (int i = 0 ; i < width ; i++)
    dot[i] = tmp + i * height;
  return (Map){.width = width, .height = height, .dot = dot};
}

void free_map_dot(Map m)
{
  free(m.dot[0]);
  free(m.dot);
}

City *load_cities(const char *filename, int *n)
{
  City *city;
  FILE *fp;
  if ((fp=fopen(filename,"rb")) == NULL){
    fprintf(stderr, "%s: cannot open file.\n",filename);
    exit(1);
  }
  fread(n,sizeof(int),1,fp);
  city = (City*)malloc(sizeof(City) * *n);
  for (int i = 0 ; i < *n ; i++){
    fread(&city[i].x, sizeof(int), 1, fp);
    fread(&city[i].y, sizeof(int), 1, fp);
  }
  fclose(fp);
  return city;
}
int main(int argc, char**argv)
{
  // const による定数定義
  const int width = 70;
  const int height = 40;
  const int max_cities = 100;

  Map map = init_map(width, height);
  
  FILE *fp = stdout; // とりあえず描画先は標準出力としておく
  if (argc != 2){
    fprintf(stderr, "Usage: %s <city file>\n", argv[0]);
    exit(1);
  }
  int n;
  City *city = load_cities(argv[1],&n);
  assert( n > 1 && n <= max_cities); // さすがに都市数100は厳しいので
  int *opt_route =(int *) malloc(sizeof(int) * n);
  double ansdp =solvedp(city,n, opt_route);
  printf("total distance = %lf\n", ansdp);
  for (int i=0;i<n;i++) {
        printf("%d -> ",opt_route[n-i-1]);
    }
  printf("0\n");
  free(opt_route);
  free(city);
  
  return 0;
}

// 繋がっている都市間に線を引く
void draw_line(Map map, City a, City b)
{
  const int n = max(abs(a.x - b.x), abs(a.y - b.y));
  for (int i = 1 ; i <= n ; i++){
    const int x = a.x + i * (b.x - a.x) / n;
    const int y = a.y + i * (b.y - a.y) / n;
    if (map.dot[x][y] == ' ') map.dot[x][y] = '*';
  }
}

void draw_route(Map map, City *city, int n, const int *route)
{
  if (route == NULL) return;

  for (int i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n];// n は 0に戻る必要あり
    draw_line(map, city[c0], city[c1]);
  }
}

void plot_cities(FILE *fp, Map map, City *city, int n, const int *route)
{
  fprintf(fp, "----------\n");

  memset(map.dot[0], ' ', map.width * map.height); 

  // 町のみ番号付きでプロットする
  for (int i = 0; i < n; i++) {
    char buf[100];
    sprintf(buf, "C_%d", i);
    for (int j = 0; j < strlen(buf); j++) {
      const int x = city[i].x + j;
      const int y = city[i].y;
      map.dot[x][y] = buf[j];
    }
  }

  draw_route(map, city, n, route);

  for (int y = 0; y < map.height; y++) {
    for (int x = 0; x < map.width; x++) {
      const char c = map.dot[x][y];
      fputc(c, fp);
    }
    fputc('\n', fp);
  }
  fflush(fp);
}

double distance(City a, City b)
{
  const double dx = a.x - b.x;
  const double dy = a.y - b.y;
  return sqrt(dx * dx + dy * dy);
}


double solvedp(const City *city, int n,int *opt_route) {
    double **dp =(double **) malloc(sizeof(double*) * 1<<n);
    for (int i=0;i < 1<<n; i++) {
        dp[i] = (double *) malloc(sizeof(double) * n);
        for (int j=0;j<n;j++) {
            dp[i][j] =-1;
        }
    }

    double res = dfs(0,0,city,dp,n);
    opt_route =best_route(dp,n,city,opt_route);
    for (int i=0;i< 1<<n; i++) {
        free(dp[i]);
    }
    free(dp);
    return res;

}

double dfs(int state,int node,const City *city,double **dp,int n) {
    dp[(1<<n)-1][0] =0;
    if (dp[state][node] != -1) {
        return dp[state][node];
    }
    if ((state == ( (1<<n) -1 ) ) && (node == 0 )) {
        return 0;
    }
    double res = inf;
    for (int i=0;i<n;i++) {
        if ( ((state>>i) & 1) ==0) {
            res =min(res,distance(city[i] , city[node]) + dfs(state|1<<i, i, city, dp, n) );
        }
    }
    dp[state][node] =res;
    return res;
}
//state　訪問された頂点の集合を管理する整数
//node　今いる頂点

int *best_route(double **dp,const int n,const City *city,int *opt_route) {
    int state = 0;
    int node =0;
    int num =0;
    while (1) {
        int flag =1;
        for (int i=0;i<n;i++) {
            if (! flag) {
                break;
            }
            if ((state & (1<<i)) == 0) {
                if (fabs(-dp[state][node] + dp[state|(1<<i)][i] +distance(city[node],city[i]))<0.00001) {
                    opt_route[num] = i;
                    state = state | 1<<i;
                    node =i;
                    num+=1;
                    flag =0;
                }
            }
        }
        if (state == ((1<<n) -1)) {
            break;
        }
    }
    return opt_route;
}