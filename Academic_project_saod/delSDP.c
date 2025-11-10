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


Node* findMin(Node* root) {
    while (root->left) root = root->left;
    return root;
}

Node* deleteNode(Node* root, int key) {
    if (!root) return NULL;
    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {

        if (!root->left) {
            Node* tmp = root->right;
            free(root);
            return tmp;
        } else if (!root->right) {
            Node* tmp = root->left;
            free(root);
            return tmp;
        }
        Node* minNode = findMin(root->right);
        root->key = minNode->key;
        root->right = deleteNode(root->right, minNode->key);
    }
    return root;
}

void postorder(Node* root) {
    if (!root) return;
    printf("%d ", root->key);
    postorder(root->left);
    postorder(root->right);
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
        data[i] = rand() % 100;
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


    printf("Обход слева направо:\n");
    inorder(sdp1);
    printf("\n\n");


    printf("сверху вниз:\n");
    postorder(sdp1);
    printf("\n\n");



    for (int i = 0; i < 10; i++) {
         printf("\nУдалим 5 элементов:\n");
        int key=0;
        scanf("%d", &key);
        printf("Удаляем %d...\n", key);
        sdp1 = deleteNode(sdp1, key);
        printf("Текущее дерево слева направо: ");
        inorder(sdp1);
        printf("\n");
    }

    freeTree(sdp1);
    freeTree(sdp2);
    freeTree(isdp);

    return 0;
}









