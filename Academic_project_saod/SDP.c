#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

typedef struct Node {
    int key;
    struct Node *left, *right;
} Node;

Node* insertRecursive(Node* root, int key) {
    if (!root) {
        Node* node = (Node*)malloc(sizeof(Node));
        node->key = key;
        node->left = node->right = NULL;
        return node;
    }
    if (key < root->key)
        root->left = insertRecursive(root->left, key);
    else if (key >= root->key)
        root->right = insertRecursive(root->right, key);
    return root;
}

void insertDoubleIndirect(Node** root, int key) {
    while (*root != NULL) {
        if (key < (*root)->key)
            root = &((*root)->left);
        else if (key >= (*root)->key)
            root = &((*root)->right);
    
    }
    *root = (Node*)malloc(sizeof(Node));
    (*root)->key = key;
    (*root)->left = (*root)->right = NULL;
}

void inorder(Node* root) {
    if (!root) return;
    inorder(root->left);
    printf("%d ", root->key);
    inorder(root->right);
}

int size(Node* root) {
    if (!root) return 0;
    return 1 + size(root->left) + size(root->right);
}

long long controlSum(Node* root) {
    if (!root) return 0;
    return root->key + controlSum(root->left) + controlSum(root->right);
}

int height(Node* root) {
    if (!root) return 0;
    int hl = height(root->left);
    int hr = height(root->right);
    return (hl > hr ? hl : hr) + 1;
}

int sumDepth(Node* root, int depth) {
    if (!root) return 0;
    return depth + sumDepth(root->left, depth + 1) + sumDepth(root->right, depth + 1);
}

Node* buildBalancedBST(int* arr, int l, int r) {
    if (l > r) return NULL;
    int m = (l + r) / 2;
    Node* root = (Node*)malloc(sizeof(Node));
    root->key = arr[m];
    root->left = buildBalancedBST(arr, l, m - 1);
    root->right = buildBalancedBST(arr, m + 1, r);
    return root;
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    srand(time(NULL));
    int data[N];
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 1000;
    }

    Node* sdp1 = NULL;
    Node* sdp2 = NULL;

    for (int i = 0; i < N; i++) {
        sdp1 = insertRecursive(sdp1, data[i]);
        insertDoubleIndirect(&sdp2, data[i]);
    }

    int arr[N];
    for (int i = 0; i < N; i++) arr[i] = data[i];
    qsort(arr, N, sizeof(int), compare);
    Node* isdp = buildBalancedBST(arr, 0, N - 1);

    printf("Обход слева направо (ISDP): \n");
    inorder(isdp);
    printf("\n\n");

    printf("Обход слева направо (SDP recursive):\n");
    inorder(sdp1);
    printf("\n\n");

    printf("Обход слева направо (SDP двойная косвеннсть):\n");
    inorder(sdp2);
    printf("\n\n");

    printf("n=%d | Размер | Контр. сумма | Высота | Ср. высота\n", N);

    int size1 = size(isdp);
    long long sum1 = controlSum(isdp);
    int h1 = height(isdp);
    double avgH1 = (double)sumDepth(isdp, 1) / size1;
    printf("ИСДП  | %d | %lld | %d | %.2f\n", size1, sum1, h1, avgH1);

    int size2 = size(sdp1);
    long long sum2 = controlSum(sdp1);
    int h2 = height(sdp1);
    double avgH2 = (double)sumDepth(sdp1, 1) / size2;
    printf(" цыПочка1 ядро - ведро  | %d | %lld | %d | %.2f\n", size2, sum2, h2, avgH2);

    int size3 = size(sdp2);
    long long sum3 = controlSum(sdp2);
    int h3 = height(sdp2);
    double avgH3 = (double)sumDepth(sdp2, 1) / size3;
    printf("цыПочка2  | %d | %lld | %d | %.2f\n", size3, sum3, h3, avgH3);

    freeTree(sdp1);
    freeTree(sdp2);
    freeTree(isdp);

    return 0;
}