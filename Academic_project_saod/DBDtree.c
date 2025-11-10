#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include<time.h>


// Алгоритм Фишера–Йетса
void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);   
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}



typedef struct NodeB {
    int Data;
    int Bal; 
    struct NodeB *Left, *Right;
} NodeB;

int VR = 1, HR = 1;

NodeB* make_nodeB(int D) {
    NodeB* p = (NodeB*)malloc(sizeof(NodeB));
    p->Data = D;
    p->Left = p->Right = NULL;
    p->Bal = 0;
    return p;
}

void B2INSERT(int D, NodeB **p) {
    NodeB *q;

    if (*p == NULL) {
        *p = make_nodeB(D);
        VR = 1;
        HR = 1;
        return;
    }

    else if ((*p)->Data > D) {  
        B2INSERT(D, &(*p)->Left);
        if (VR == 1) {
            if ((*p)->Bal == 0) {
                q = (*p)->Left;
                (*p)->Left = q->Right;
                q->Right = *p;
                *p = q;
                (*p)->Bal = 1;
                VR = 0;
                HR = 1;
            } else {
                (*p)->Bal = 0;
                VR = 1;
                HR = 0;
            }
        } else HR = 0;
    }

    else if ((*p)->Data < D) {     
        B2INSERT(D, &(*p)->Right);
        if (VR == 1) {
            (*p)->Bal = 1;
            HR = 1;
            VR = 0;
        } 
        else if (HR == 1) {
            if ((*p)->Bal == 1) {
                q = (*p)->Right;
                (*p)->Bal = 0;
                q->Bal = 0;
                (*p)->Right = q->Left;
                q->Left = *p;
                *p = q;
                VR = 1;
                HR = 0;
            } else HR = 0;
        }
    }
    else { 
        VR = 0;
        HR = 0;
    }
}

int treeSizeB(NodeB* root) {
    if (!root) return 0;
    return 1 + treeSizeB(root->Left) + treeSizeB(root->Right);
}
int checkSumB(NodeB* root) {
    if (!root) return 0;
    return root->Data + checkSumB(root->Left) + checkSumB(root->Right);
}
int treeHeightB(NodeB* root) {
    if (!root) return 0;
    int l = treeHeightB(root->Left);
    int r = treeHeightB(root->Right);
    return 1 + (l > r ? l : r);
}
int sumHeightsB(NodeB* root, int depth) {
    if (!root) return 0;
    return depth + sumHeightsB(root->Left, depth + 1) + sumHeightsB(root->Right, depth + 1);
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
    } 
    else if (D > p->data) {
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
    } 
    else *Rost = false;
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

/* ===================== MAIN ===================== */
int main() {
    srand(time(NULL));
    int a[100];

    for (int i = 0; i < 100; ++i) a[i] = i + 1;
    shuffle(a, 100);
    int n = 100;

    NodeB* dbd = NULL;
    Nodea* avl = NULL;

    for (int i = 0; i < n; ++i) {
        B2INSERT(a[i], &dbd);
        avl_insert(&avl, a[i]);
    }

    int size_dbd   = treeSizeB(dbd);
    int sum_dbd    = checkSumB(dbd);
    int h_dbd      = treeHeightB(dbd);
    int sh_dbd     = sumHeightsB(dbd, 1);
    double avg_dbd = (double)sh_dbd / size_dbd;

    int size_avl   = treeSizeA(avl);
    int sum_avl    = checkSumA(avl);
    int h_avl      = treeHeightA(avl);
    int sh_avl     = sumHeightsA(avl, 1);
    double avg_avl = (double)sh_avl / size_avl;

    printf("n=%d\t%-8s\t%-13s\t%-6s\t%-13s\n", n, "Размер", "Контр.Сумма", "Высота", "Средн.высота");
    printf("АВЛ\t%-8d\t%-13d\t%-6d\t%-13.2f\n", size_avl,  sum_avl,  h_avl,  avg_avl);
    printf("ДБД\t%-8d\t%-13d\t%-6d\t%-13.2f\n", size_dbd,  sum_dbd,  h_dbd,  avg_dbd);

    return 0;
}
