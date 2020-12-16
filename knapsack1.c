#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> // strtol, strtod, strerror
#include <errno.h> // strtol, strtod でerror を補足したい

// 構造体 Item
typedef struct item
{
  double value;
  double weight;
}Item;

// 構造体 Itemset
typedef struct itemset
{
  int number;
  Item *item;
} Itemset;

// 解を表す構造体
typedef struct ans
{
  double value;
  unsigned char *flags;
} Answer;

// 関数のプロトサイプ宣言
Itemset *init_itemset(int number, int seed);
void free_itemset(Itemset *list);
Itemset *load_itemset(char *filename);
void print_itemset(const Itemset *list);
void save_itemset(char *filename);

Answer solve(const Itemset *list, double capacity);
Answer search(int index, const Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w);

// エラー判定付きの読み込み関数
int load_int(const char *argvalue);
double load_double(const char *argvalue);

int load_int(const char *argvalue)
{
  long nl;
  char *e;
  errno = 0; // errno.h で定義されているグローバル変数を一旦初期化
  nl = strtol(argvalue,&e,10);
  if (errno == ERANGE){
    fprintf(stderr,"%s: %s\n",argvalue,strerror(errno));
    exit(1);
  }
  if (*e != '\0'){
    fprintf(stderr,"%s: an irregular character '%c' is detected.\n",argvalue,*e);
    exit(1);
  }
  return (int)nl;
}
double load_double(const char *argvalue)
{
  double ret;
  char *e;
  errno = 0; // errno.h で定義されているグローバル変数を一旦初期化
  ret = strtod(argvalue,&e);
  if (errno == ERANGE){
    fprintf(stderr,"%s: %s\n",argvalue,strerror(errno));
    exit(1);
  }
  if (*e != '\0'){
    fprintf(stderr,"%s: an irregular character '%c' is detected.\n",argvalue,*e);
    exit(1);
  }
  return ret;
}

// main関数
// プログラム使用例: ./knapsack 10 20
//  10個の品物を設定し、キャパ20 でナップサック問題をとく
int main (int argc, char**argv)
{
  /* 引数処理: ユーザ入力が正しくない場合は使い方を標準エラーに表示して終了 */
  if (argc != 3){
    fprintf(stderr, "usage: %s <filename> <max capacity (double)>\n",argv[0]);
    exit(1);
  }
  FILE fp;
  if ((fp = fopen(argv[1],"r") == NULL)) {
      printf("Cannot open the file %s",argv[1]);
  }

  
  // 個数の上限はあらかじめ定めておく
  const int max_items = 100;

  //const int n = load_int(argv[1]);
  assert( n <= max_items ); // assert で止める

  const double W = load_double(argv[2]);
  assert( W >= 0.0);
  
  printf("max capacity: W = %.f, # of items: %d\n",W, n);

  // 乱数シードを1にして、初期化 (ここは変更可能)
  int seed = 1; 
  Itemset *items = init_itemset(n, seed);
  print_itemset(items);

  // ソルバーで解く
  Answer a = solve(items, W);
  
  // 表示する
  printf("----\nbest solution:\n");
  printf("value: %4.1f\n",a.value);
  for (int i = 0 ; i < items->number ; i++){
    printf("%d",a.flags[i]);
  }
  printf("\n");
  free(a.flags);
  a.flags = NULL;
  
  free_itemset(items);
  return 0;
}

// 構造体をポインタで確保するお作法を確認してみよう
Itemset *init_itemset(int number, int seed)
{
  Itemset *list = (Itemset*)malloc(sizeof(Itemset));

  Item *item = (Item*)malloc(sizeof(Item)*number);

  srand(seed);
  for (int i = 0 ; i < number ; i++){
    item[i].value = 0.1 * (rand() % 200);
    item[i].weight = 0.1 * (rand() % 200 + 1);
  }
  *list = (Itemset){.number = number, .item = item};
  return list;
}

// itemset の free関数
void free_itemset(Itemset *list)
{
  free(list->item);
  free(list);
}

// 表示関数
void print_itemset(const Itemset *list)
{
  int n = list->number;
  const char *format = "v[%d] = %4.1f, w[%d] = %4.1f\n";
  for(int i = 0 ; i < n ; i++){
    printf(format, i, list->item[i].value, i, list->item[i].weight);
  }
  printf("----\n");
}

// ソルバーは search を index = 0 で呼び出すだけ
Answer solve(const Itemset *list,  double capacity)
{
  unsigned char *flags = (unsigned char*)calloc(list->number, sizeof(unsigned char));
  Answer max_value = search(0,list,capacity,flags, 0.0, 0.0);
  free(flags);
  return max_value;
}

// 再帰的な探索関数
Answer search(int index, const Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w)
{
  int max_index = list->number;
  assert(index >= 0 && sum_v >= 0 && sum_w >= 0);
  // 必ず再帰の停止条件を明記する (最初が望ましい)
  if (index == max_index){
    const char *format_ok = ", total_value = %5.1f, total_weight = %5.1f\n";
    const char *format_ng = ", total_value = %5.1f, total_weight = %5.1f NG\n";
    for (int i = 0 ; i < max_index ; i++){
      printf("%d", flags[i]);
    }
  
    if (sum_w <= capacity){
      printf(format_ok, sum_v, sum_w);
      unsigned char *arg = (unsigned char*)malloc(sizeof(unsigned char) * list->number);
      memcpy(arg, flags, sizeof(unsigned char) * list->number);
      return (Answer){ .value = sum_v , .flags = arg};
    } else{
      printf(format_ng, sum_v, sum_w);
      return (Answer){ .value = 0 , .flags = NULL};
    }
  }

  // 以下は再帰の更新式: 現在のindex の品物を使う or 使わないで分岐し、index をインクリメントして再帰的にsearch() を実行する
  
  flags[index] = 0;
  const Answer a0 = search(index+1, list, capacity, flags, sum_v, sum_w);

  flags[index] = 1;
  
  Answer a1;
  if (sum_w + list->item[index].weight > capacity){
    a1 = (Answer){ .value = 0 , .flags = NULL};
  }
  else {
    a1 = search(index+1, list, capacity, flags, sum_v + list->item[index].value, sum_w + list->item[index].weight);
  }

  // 使った場合の結果と使わなかった場合の結果を比較して返す
  if ( a0.value > a1.value){
    free(a1.flags);
    return a0;
  }
  else{
    free(a0.flags);
    return a1;
  }
}
