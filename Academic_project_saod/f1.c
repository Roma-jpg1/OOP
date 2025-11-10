#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>
#include <stdbool.h>

#define RECORDS_PER_PAGE 20
#define MAX_RECS 4000

/* ===== CP866 → UTF-8 ===== */
static void cp866_to_utf8(const char *cp, char *u8, size_t cap) {
    size_t j = 0;
    for (size_t i = 0; cp[i] && j + 2 < cap; i++) {
        unsigned char c = (unsigned char)cp[i];
        if (c >= 0x80 && c <= 0xAF) { u8[j++] = 0xD0; u8[j++] = 0x90 + (c - 0x80); }
        else if (c >= 0xE0 && c <= 0xEF) { u8[j++] = 0xD1; u8[j++] = 0x80 + (c - 0xE0); }
        else if (c == 0xF0) { u8[j++] = 0xD0; u8[j++] = 0x81; }
        else if (c == 0xF1) { u8[j++] = 0xD1; u8[j++] = 0x91; }
        else { u8[j++] = c; }
    }
    u8[j] = 0;
}

static int u8_clen(unsigned char c){
    if((c&0x80)==0) return 1;
    if((c&0xE0)==0xC0) return 2;
    if((c&0xF0)==0xE0) return 3;
    if((c&0xF8)==0xF0) return 4;
    return 1;
}
static int u8_strlen_chars(const char *s){
    int n=0; for(size_t i=0;s[i];){ i+=u8_clen((unsigned char)s[i]); n++; } return n;
}
static void u8_upper_ru(char *s){
    for(size_t i=0;s[i];){
        unsigned char a=s[i], b=s[i+1];
        if((a&0xE0)==0xC0 && b){
            if(a==0xD0 && b>=0xB0 && b<=0xBF){ s[i]=0xD0; s[i+1]=b-0x20; i+=2; continue; }
            if(a==0xD1 && b>=0x80 && b<=0x8F){ s[i]=0xD0; s[i+1]=b+0x20; i+=2; continue; }
            if(a==0xD1 && b==0x91){ s[i]=0xD0; s[i+1]=0x81; i+=2; continue; }
            i+=2;
        }else i++;
    }
}

struct record {
    char author[12];
    char title[32];
    char publisher[16];
    short year;
    short num_of_page;
};

typedef struct Node {
    struct record data;
    float weight;
    struct Node *left, *right;
} Node;

typedef struct {
    struct record items[MAX_RECS];
    int front, rear;
} Queue;

static Queue found_queue;
static void init_queue(Queue *q){ q->front=q->rear=0; }
static bool is_empty_queue(Queue *q){ return q->front==q->rear; }
static void enqueue_record(Queue *q, struct record r){ q->items[q->rear++]=r; }
static struct record dequeue_record_only(Queue *q){ return q->items[q->front++]; }

static void field_to_utf8_pretty(const char* bytes,int len,char*out,int cap){
    char tmp[256];
    int L=len; while(L>0&&bytes[L-1]==0)L--;
    if(L<0)L=0; if(L>(int)sizeof(tmp)-1)L=(int)sizeof(tmp)-1;
    memcpy(tmp,bytes,L); tmp[L]=0;
    for(int i=0;i<L;i++) if(tmp[i]=='_') tmp[i]=' ';
    cp866_to_utf8(tmp,out,cap);
}

static void surname_utf8_from_title(const char* title,char*out,int cap){
    char utf8[256];
    field_to_utf8_pretty(title,32,utf8,sizeof(utf8));
    int len=(int)strlen(utf8);
    int end=len-1; while(end>=0&&isspace((unsigned char)utf8[end]))end--;
    int i=end; while(i>=0&&!isspace((unsigned char)utf8[i]))i--;
    int start=i+1; int wl=end-start+1;
    if(wl<=0){out[0]=0;return;}
    if(wl>=cap)wl=cap-1;
    memcpy(out,utf8+start,(size_t)wl);
    out[wl]=0; u8_upper_ru(out);
}

static int cmp_by_surname_title(const struct record* a, const struct record* b){
    char sa[128], sb[128];
    surname_utf8_from_title(a->title, sa, sizeof(sa));
    surname_utf8_from_title(b->title, sb, sizeof(sb));
    int c=strcmp(sa,sb);
    if(c!=0)return c;
    return memcmp(a->title,b->title,32);
}
static void quicksort_hoare(struct record arr[],int L,int R){
    if(L>=R)return;
    int i=L,j=R;struct record piv=arr[(L+R)/2];
    do{
        while(cmp_by_surname_title(&arr[i],&piv)<0)i++;
        while(cmp_by_surname_title(&arr[j],&piv)>0)j--;
        if(i<=j){struct record t=arr[i];arr[i]=arr[j];arr[j]=t;i++;j--;}
    }while(i<=j);
    if(L<j)quicksort_hoare(arr,L,j);
    if(i<R)quicksort_hoare(arr,i,R);
}

static int search_prefix_to_queue(struct record *arr,int n,const char *pref,Queue *q){
    init_queue(q);
    if(n==0||!pref||pref[0]=='\0')return 0;
    size_t plen=strlen(pref);
    int left=0,right=n-1,mid,found=-1;
    while(left<=right){
        mid=(left+right)/2;
        char sm[128]; surname_utf8_from_title(arr[mid].title,sm,sizeof(sm));
        int cmp=strncmp(sm,pref,plen);
        if(cmp==0){found=mid;break;}
        else if(cmp<0)left=mid+1;
        else right=mid-1;
    }
    if(found==-1)return 0;
    int s=found,e=found;
    while(s>0){
        char sm[128]; surname_utf8_from_title(arr[s-1].title,sm,sizeof(sm));
        if(strncmp(sm,pref,plen)==0)s--;else break;
    }
    while(e<n-1){
        char sm[128]; surname_utf8_from_title(arr[e+1].title,sm,sizeof(sm));
        if(strncmp(sm,pref,plen)==0)e++;else break;
    }
    for(int i=s;i<=e;i++)enqueue_record(q,arr[i]);
    return e-s+1;
}

static void assign_weights_by_year(struct record *arr,float *w,int n) {
    if (n == 0) return;
    int min_year = arr[0].year, max_year = arr[0].year;
    for (int i = 1; i < n; i++) {
        if (arr[i].year < min_year) min_year = arr[i].year;
        if (arr[i].year > max_year) max_year = arr[i].year;
    }
    for (int i = 0; i < n; i++)
        w[i] = (float)(max_year - arr[i].year + 1); 
}

static Node* buildA2(struct record *arr,float *w,int n){
    if(n==0)return NULL;
    int max_i=0;
    for(int i=1;i<n;i++) if(w[i]>w[max_i]) max_i=i;
    Node*node=malloc(sizeof(Node));
    node->data=arr[max_i];
    node->weight=w[max_i];
    node->left=buildA2(arr,w,max_i);
    node->right=buildA2(arr+max_i+1,w+max_i+1,n-max_i-1);
    return node;
}

static Node* find_by_year(Node *root, int year, struct record *out, float *weights, int *count) {
    if (!root) return;

    find_by_year(root->left, year, out, weights, count);

    if (root->data.year == year) {
        out[*count] = root->data;
        weights[*count] = root->weight;
        (*count)++;
    }

    find_by_year(root->right, year, out, weights, count);
}

static void inorder_collect(Node *r,struct record*out,float*w,int*cnt){
    if(!r)return;
    inorder_collect(r->left,out,w,cnt);
    out[*cnt]=r->data; w[*cnt]=r->weight; (*cnt)++;
    inorder_collect(r->right,out,w,cnt);
}

static void print_line(int n){ for(int i=0;i<n;i++)putchar('-'); putchar('\n'); }

static void display_records_with_weights(struct record*arr,const float*w,int count){
    int W_IDX=4,W_AUTHOR=12,W_TITLE=32,W_PUBLISHER=16,W_YEAR=4,W_PAGES=6;
    int base=W_IDX+3+W_AUTHOR+3+W_TITLE+3+W_PUBLISHER+3+W_YEAR+3+W_PAGES+3+6;
    int pos=0;
    while(pos<count){
        print_line(base);
        printf("%-3s | %-12s | %-32s | %-16s | %-4s | %-6s | %-6s\n",
               "№","Автор","Заглавие","Издательство","Год","Стр.","Вес");
        print_line(base);
        for(int i=0;i<RECORDS_PER_PAGE&&pos<count;i++,pos++){
            char au[256],ti[256],pu[256];
            field_to_utf8_pretty(arr[pos].author,12,au,sizeof(au));
            field_to_utf8_pretty(arr[pos].title,32,ti,sizeof(ti));
            field_to_utf8_pretty(arr[pos].publisher,16,pu,sizeof(pu));
            printf("%-3d | %-12s | %-32s | %-16s | %4d | %6d | %6.0f\n",
                   pos+1,au,ti,pu,arr[pos].year,arr[pos].num_of_page,w?w[pos]:0);
        }
        if(pos<count){
            printf("\nПоказать следующие %d записей? (y/n): ",RECORDS_PER_PAGE);
            int c=getchar(); while(getchar()!='\n');
            if(tolower(c)!='y')break;
        }
    }
}

int main(void){
    setlocale(LC_ALL,"ru_RU.UTF-8");
    srand((unsigned)time(NULL));

    FILE*fp=fopen("testBase1.dat","rb");
    if(!fp){perror("testBase1.dat");return 1;}
    struct record db[MAX_RECS];
    int n=fread(db,sizeof(struct record),MAX_RECS,fp);
    fclose(fp);

    struct record sorted[MAX_RECS];
    Node *root = NULL;
    init_queue(&found_queue);

    int choice;
    do{
        printf("\n=== МЕНЮ ===\n");
        printf("1. Показать базу\n");
        printf("2. Сортировать по фамилии (title)\n");
        printf("3. Поиск по фамилии (в очередь)\n");
        printf("4. Построить дерево A2 по году\n");
        printf("5. Поиск по году в дереве\n");
        printf("0. Выход\n> ");
        if(scanf("%d",&choice)!=1){while(getchar()!='\n');continue;}
        while(getchar()!='\n');

        switch(choice){
        case 1:
            display_records_with_weights(db,NULL,n);
            break;

        case 2:
            memcpy(sorted,db,sizeof(struct record)*n);
            quicksort_hoare(sorted,0,n-1);
            display_records_with_weights(sorted,NULL,n);
            break;

        case 3:{
            memcpy(sorted,db,sizeof(struct record)*n);
            quicksort_hoare(sorted,0,n-1);
            char pref[128];
            printf("Введите префикс фамилии: ");
            if(!fgets(pref,sizeof(pref),stdin))break;
            pref[strcspn(pref,"\r\n")]=0;
            u8_upper_ru(pref);
            int found=search_prefix_to_queue(sorted,n,pref,&found_queue);
            if(found<=0){printf("Ничего не найдено.\n");break;}
            printf("\nНайдено %d записей, добавлены в очередь.\n",found);
            display_records_with_weights(found_queue.items,NULL,found_queue.rear);
        }break;

        case 4:{
            if(is_empty_queue(&found_queue)){
                printf("Очередь пуста. Сначала выполните поиск.\n"); break;
            }
            int size=found_queue.rear-found_queue.front;
            struct record recs[MAX_RECS]; float weights[MAX_RECS];
            for(int i=0;i<size;i++) recs[i]=dequeue_record_only(&found_queue);
            assign_weights_by_year(recs,weights,size);

            root=buildA2(recs,weights,size);
            struct record inord[MAX_RECS]; float inw[MAX_RECS]; int cnt=0;
            inorder_collect(root,inord,inw,&cnt);
            printf("\n=== ДЕРЕВО A2 (ПО ГОДАМ) ===\n");
            display_records_with_weights(inord,inw,cnt);
        }break;

        case 5: {
            if (!root) {
                printf("Сначала постройте дерево (пункт 4)\n");
                break;
            }

            int year;
            printf("Введите год для поиска: ");
            if (scanf("%d", &year) != 1) { while (getchar() != '\n'); break; }
            while (getchar() != '\n');

            struct record found[MAX_RECS];
            float found_w[MAX_RECS];
            int count = 0;

            find_by_year(root, year, found, found_w, &count);

            if (count == 0) {
                printf("Книги за %d год не найдены.\n", year);
            } else {
                printf("\nНайдено %d книг(и) за %d год:\n", count, year);
                display_records_with_weights(found, found_w, count);
            }
        } break;
        case 0: printf("Выход...\n"); break;
        default: printf("Неверный пункт.\n");
        }
    }while(choice!=0);
    return 0;
}
