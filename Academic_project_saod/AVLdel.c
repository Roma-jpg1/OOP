#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct NodeA {
    int data;
    int Bal;
    struct NodeA *Left, *Right;
} Nodea;

Nodea* make_node(int x) {
    Nodea* p = (Nodea*)malloc(sizeof(Nodea));
    p->data = x; p->Left = p->Right = NULL; p->Bal = 0;
    return p;
}

Nodea* rotate_right(Nodea* p) {
    Nodea* q = p->Left;
    p->Left = q->Right;
    q->Right = p;
    return q;
}

Nodea* rotate_left(Nodea* p) {
    Nodea* q = p->Right;
    p->Right = q->Left;
    q->Left = p;
    return q;
}

Nodea* LL(Nodea* p) {
    Nodea* q = p->Left;
    p = rotate_right(p);
    q->Bal = 0; p->Right->Bal = 0;
    return p;
}

Nodea* RR(Nodea* p) {
    Nodea* q = p->Right;
    p = rotate_left(p);
    q->Bal = 0; p->Left->Bal = 0;
    return p;
}

Nodea* LR(Nodea* p) {
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

Nodea* RL(Nodea* p) {
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

void insert_avl(Nodea** pp, int D, bool* Rost) {
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
    } else *Rost = false;
}

void avl_insert(Nodea** root, int x) {
    bool growth = false;
    insert_avl(root, x, &growth);
}

Nodea* LL1(Nodea* p, bool* umensh) {
    Nodea* q = p->Left;
    if (q->Bal == 0) { 
        q->Bal = +1; 
        p->Bal = -1; 
        *umensh = false; }
    else { 
        q->Bal = 0; 
        p->Bal = 0; }
    p->Left = q->Right; 
    q->Right = p;
    return q;
}

Nodea* RR1(Nodea* p, bool* umensh) {
    Nodea* q = p->Right;
    if (q->Bal == 0) { 
        q->Bal = -1; 
        p->Bal = +1; 
        *umensh = false; }
    else { q->Bal = 0; 
        p->Bal = 0; }
    p->Right = q->Left; 
    q->Left = p;
    return q;
}

void BL(Nodea** pp, bool* umensh) {
    Nodea* p = *pp;
    if (p->Bal == -1)       
        { p->Bal = 0; 
        *umensh = true; }
    else if (p->Bal == 0)   
        { p->Bal = +1;
        *umensh = false; }
    else {
        if (p->Right->Bal >= 0) 
            *pp = RR1(p, umensh);
        else                    
        { *pp = RL(p); 
        *umensh = true; }
    }
}

static void BR(Nodea** pp, bool* umensh) {
    Nodea* p = *pp;
    if (p->Bal == +1)       
        { p->Bal = 0; 
        *umensh = true; }
    else if (p->Bal == 0)   
        { p->Bal = -1; 
        *umensh = false; }
    else {
        if (p->Left->Bal <= 0) *pp = LL1(p, umensh);
        else                   
        { *pp = LR(p);
        *umensh = true; }
    }
}

static void del_pred(Nodea** rr, int* outData, bool* umensh) {
    Nodea* r = *rr;
    if (r->Right != NULL) {
        del_pred(&r->Right, outData, umensh);
        if (*umensh) BR(rr, umensh);
    } else {
        *outData = r->data;
        Nodea* q = r;
        r = r->Left;
        *rr = r;
        free(q);
        *umensh = true;
    }
}

static void delete_avl(Nodea** pp, int x, bool* umensh) {
    Nodea* p = *pp;
    if (!p) { *umensh = false; return; }

    if (x < p->data) {
        delete_avl(&p->Left, x, umensh);
        if (*umensh) BL(pp, umensh);
    } else if (x > p->data) {
        delete_avl(&p->Right, x, umensh);
        if (*umensh) BR(pp, umensh);
    } else {
        Nodea* q = p;
        if (q->Left == NULL) { *pp = q->Right; free(q); *umensh = true; return; }
        else if (q->Right == NULL) { *pp = q->Left; free(q); *umensh = true; return; }
        else {
            int pred;
            del_pred(&p->Left, &pred, umensh);
            p->data = pred;
            if (*umensh) BL(pp, umensh);
        }
    }
}

void avl_delete(Nodea** root, int x) {
    bool um = false;
    delete_avl(root, x, &um);
}

void inorder(Nodea* p) {
    if (!p) return;
    inorder(p->Left);
    printf("%d ", p->data);
    inorder(p->Right);
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

    Nodea* root = NULL;
    for (int i = 0; i < 100; i++) avl_insert(&root, a[i]);
    inorder(root);
    for (int i = 0; i < 10; i++) {
        int key;
        if (scanf("%d", &key) != 1) break;
        avl_delete(&root, key);
        inorder(root);
        printf("\n");
    }
    return 0;
}
