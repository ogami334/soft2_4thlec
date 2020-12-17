#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#include <time.h>

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
Map init_map(const int width, const int height);
void free_map_dot(Map m);
City *load_cities(const char* filename,int *n);
int* generate_patterns(int n);//n-1個の都市を訪れる順番を決める関数
Answer local_optimize(int n,int *pattern,City *city);
double calculate_total_distance(int n,int *pattern,City *city);
void swap (int n,int *pattern,int i,int j);



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

void swap (int n,int *pattern,int i,int j);

double calculate_total_distance(int n,int *pattern,City *city) {
    double val =0;
    val +=distance(city[0],city[pattern[0]]);
    val +=distance(city[pattern[n-1]],city[0]);
    for (int i=0;i<n-2;i++) {
        val +=distance(city[pattern[i]],city[pattern[i+1]]);
    }

}//パターンが与えられたときに距離を返す関数O(n)
int* generate_patterns(int n) {
    int *pattern = (int *) malloc (sizeof(int) * (n-1));
    int flag[n-1];
    for (int i=0;i<n-1;i++) {
        flag[i]=0;
    }
    int index =0;
    while (index < n-1)  {
        printf("%d\n",index);
        int s =rand()%(n-1) +1;
        if (flag[s-1] == 0) {
            pattern[index] =s;
            index +=1;
            flag[s-1]=1;
        }
    }
    return pattern;
}

Answer local_optimize(int n, int *pattern, City *city) {
    int flag_optim =1;
    double minval =calculate_total_distance(n,pattern,city);
    int *record =(int *) malloc (sizeof(int) * (n-1) );
    while (!flag_optim) {
        flag_optim =1;//いったん1にしておき、より良い局所解が見つかれば0に更新
        for (int i=0;i<n-1;i++) {
            for (int j=i+1;j<n-1;j++) {
              

                
            }
        }
    }//cityが必要
    return (Answer) {.sum_d = ,.pattern =};
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
  srand((unsigned int)time(NULL));
  int *pattern =generate_patterns(n);
  /*for (int i=0;i<n;i++) {
      printf("%d ",pattern[i]);
  }
  printf("\n");*/
  // 町の初期配置を表示
  plot_cities(fp, map, city, n, NULL);
  sleep(1);

  // 訪れる順序を記録する配列を設定
  int *route = (int*)calloc(n, sizeof(int));
  // 訪れた町を記録するフラグ
  int *visited = (int*)calloc(n, sizeof(int));

  const double d = solve(city,n,route,visited);
  plot_cities(fp, map, city, n, route);
  printf("total distance = %f\n", d);
  for (int i = 0 ; i < n ; i++){
    printf("%d -> ", route[i]);
  }
  printf("0\n");

  // 動的確保した環境ではfreeをする
  free(route);
  free(visited);
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

typedef struct ans{
  double dist;
  int *route;
} Answer;

Answer search(const City *city, int n, int *route, int *visited);

double solve(const City *city, int n, int *route, int *visited)
{
  route[0] = 0; // 循環した結果を避けるため、常に0番目からスタート
  visited[0] = 1;

  Answer ans = search(city, n, route, visited);
  
  memcpy(route, ans.route, sizeof(int)*n);
  free(ans.route);
  return ans.dist;
}

Answer search(const City *city, int n, int *route, int *visited){
  int start = 0;
  // 訪問した個数を数える
  for (int i = 1; i < n ; i++){
    if (!route[i]){
      start = i;
      break;
    }
  }
  // 全て訪問したケース（ここが再帰の終端条件）
  if (start == 0){
    double sum_d = 0;
    for (int i = 0 ; i < n ; i++){
      const int c0 = route[i];
      const int c1 = route[(i+1)%n]; // nは0に戻る
      sum_d += distance(city[c0],city[c1]);
    }
    int *retarg = (int*)malloc(sizeof(int)*n);
    memcpy(retarg, route, sizeof(int)*n);
    
    return (Answer){.dist = sum_d, .route = retarg};
  }


  // 特定の分岐における最小の巡回経路を調べる
  Answer min = {.dist = 10000000000, .route = NULL};
  for (int i = 1 ; i < n ; i++){
    // 未訪問なら訪れる
    if(!visited[i]){
      if(i == 2 && !visited[1]) continue; // 逆順の巡回経路を抑制

      route[start] = i; 
      visited[i] = 1;

      Answer tmp = search(city, n, route, visited);

      // 最小の巡回経路かどうか確認
      if ( tmp.dist < min.dist ){
	free(min.route);
	min = tmp;
      }
      else{
	free(tmp.route);
      }

      route[start] = 0;
      visited[i] = 0;
    }
  }
  
  return min;
}

//main関数ないでランダムに初期値を生成(random_generate関数)
//local_optimize関数で局所最適解を返す
//今まで得られた解と比較