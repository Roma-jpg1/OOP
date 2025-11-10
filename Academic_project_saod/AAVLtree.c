#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node* buildISDP(const int *arr, int l, int r) {
    if (l > r) return NULL;
    int m = l + (r - l) / 2;           
    Node* node = createNode(arr[m]);
    node->left  = buildISDP(arr, l,   m - 1);
    node->right = buildISDP(arr, m + 1, r);
    return node;
}

int treeSize(Node* root) {
    if (!root) return 0;
    return 1 + treeSize(root->left) + treeSize(root->right);
}

int checkSum(Node* root) {
    if (!root) return 0;
    return root->key + checkSum(root->left) + checkSum(root->right);
}

int treeHeight(Node* root) {
    if (!root) return 0;
    int l = treeHeight(root->left);
    int r = treeHeight(root->right);
    return 1 + (l > r ? l : r);
}

int sumHeights(Node* root, int depth) {
    if (!root) return 0;
    return depth + sumHeights(root->left, depth + 1) + sumHeights(root->right, depth + 1);
}


typedef struct NodeA {
    int data;
    int Bal;
    struct NodeA *Left, *Right;
} Nodea;

static Nodea* make_node(int x) {
    Nodea* p = (Nodea*)malloc(sizeof(Nodea));
    p->data = x; p->Left = p->Right = NULL; p->Bal = 0;
    return p;
}

static Nodea* rotate_right(Nodea* p) {
    Nodea* q = p->Left;
    p->Left = q->Right;
    q->Right = p;
    return q;
}

static Nodea* rotate_left(Nodea* p) {
    Nodea* q = p->Right;
    p->Right = q->Left;
    q->Left = p;
    return q;
}

static Nodea* LL(Nodea* p) {
    Nodea* q = p->Left;
    p = rotate_right(p);
    q->Bal = 0; p->Right->Bal = 0;
    return p;
}

static Nodea* RR(Nodea* p) {
    Nodea* q = p->Right;
    p = rotate_left(p);
    q->Bal = 0; p->Left->Bal = 0;
    return p;
}

static Nodea* LR(Nodea* p) {
    Nodea* q = p->Left;
    Nodea* r = q->Right;
    p->Left = rotate_left(q);
    p = rotate_right(p);
    if (r->Bal == -1) { p->Right->Bal = +1; p->Left->Bal  = 0; }
    else if (r->Bal == +1) { p->Left->Bal  = -1; p->Right->Bal = 0; }
    else { p->Left->Bal = p->Right->Bal = 0; }
    p->Bal = 0;
    return p;
}

static Nodea* RL(Nodea* p) {
    Nodea* q = p->Right;
    Nodea* r = q->Left;
    p->Right = rotate_right(q);
    p = rotate_left(p);
    if (r->Bal == +1) { p->Left->Bal  = -1; p->Right->Bal = 0; }
    else if (r->Bal == -1) { p->Right->Bal = +1; p->Left->Bal  = 0; }
    else { p->Left->Bal = p->Right->Bal = 0; }
    p->Bal = 0;
    return p;
}

static void insert_avl(Nodea** pp, int D, bool* Rost) {
    Nodea* p = *pp;
    if (!p) { *pp = make_node(D); *Rost = true; return; }

    if (D < p->data) {
        insert_avl(&p->Left, D, Rost);
        if (*Rost) {
            if (p->Bal == +1) { p->Bal = 0; *Rost = false; }
            else if (p->Bal == 0) { p->Bal = -1; *Rost = true; }
            else {
                if (p->Left && p->Left->Bal <= 0) *pp = LL(p);
                else *pp = LR(p);
                (*pp)->Bal = 0; *Rost = false;
            }
        }
    } else if (D > p->data) {
        insert_avl(&p->Right, D, Rost);
        if (*Rost) {
            if (p->Bal == -1) { p->Bal = 0; *Rost = false; }
            else if (p->Bal == 0) { p->Bal = +1; *Rost = true; }
            else {
                if (p->Right && p->Right->Bal >= 0) *pp = RR(p);
                else *pp = RL(p);
                (*pp)->Bal = 0; *Rost = false;
            }
        }
    } else {
        *Rost = false;
    }
}

void avl_insert(Nodea** root, int x) {
    bool growth = false;
    insert_avl(root, x, &growth);
}

int treeSizeA(Nodea* root) {
    if (!root) return 0;
    return 1 + treeSizeA(root->Left) + treeSizeA(root->Right);
}
int checkSumA(Nodea* root) {
    if (!root) return 0;
    return root->data + checkSumA(root->Left) + checkSumA(root->Right);
}
int treeHeightA(Nodea* root) {
    if (!root) return 0;
    int l = treeHeightA(root->Left), r = treeHeightA(root->Right);
    return 1 + (l > r ? l : r);
}
int sumHeightsA(Nodea* root, int depth) {
    if (!root) return 0;
    return depth + sumHeightsA(root->Left, depth + 1) + sumHeightsA(root->Right, depth + 1);
}

int cmp_int(const void *pa, const void *pb) {
    int a = *(const int*)pa, b = *(const int*)pb;
    return (a > b) - (a < b);
}

int main() {
    int a[100] = {
        57, 3, 94, 68, 12, 46, 89, 38, 82, 24,
        41, 66, 76, 20, 52, 28, 2, 30, 14, 60,
        92, 16, 85, 74, 31, 39, 6, 26, 64, 9,
        18, 49, 8, 35, 59, 50, 78, 48, 40, 1,
        97, 32, 22, 79, 87, 19, 90, 4, 54, 96,
        36, 62, 84, 11, 33, 56, 42, 15, 27, 34,
        5, 93, 10, 83, 25, 98, 53, 23, 70, 13,
        81, 45, 73, 91, 7, 80, 21, 71, 29, 95,
        99, 47, 75, 63, 44, 51, 17, 55, 61, 67,
        88, 43, 86, 77, 65, 72, 58, 100, 37, 69
    };

    int n = sizeof(a) / sizeof(a[0]);


    Nodea* avl = NULL;
    for (int i = 0; i < n; ++i) {   
        avl_insert(&avl, a[i]);
    }

    qsort(a, n, sizeof(a[0]), cmp_int);   

    Node*  isdp = buildISDP(a, 0, n - 1);

    int size_isdp   = treeSize(isdp);
    int sum_isdp    = checkSum(isdp);
    int h_isdp      = treeHeight(isdp);
    int sh_isdp     = sumHeights(isdp, 1);
    double avg_isdp = (double)sh_isdp / size_isdp;

    int size_avl   = treeSizeA(avl);
    int sum_avl    = checkSumA(avl);
    int h_avl      = treeHeightA(avl);
    int sh_avl     = sumHeightsA(avl, 1);
    double avg_avl = (double)sh_avl / size_avl;

    printf("n=%d\t%-8s\t%-13s\t%-6s\t%-13s\n", n, "Размер", "Контр.Сумма", "Высота", "Средн.высота");
    printf("ИСДП\t%-8d\t%-13d\t%-6d\t%-13.2f\n", size_isdp, sum_isdp, h_isdp, avg_isdp);
    printf("АВЛ \t%-8d\t%-13d\t%-6d\t%-13.2f\n", size_avl,  sum_avl,  h_avl,  avg_avl);

    return 0;
}
