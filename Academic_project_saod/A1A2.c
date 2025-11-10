#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

typedef struct Node {
    int key, weight;
    struct Node *left, *right;
} Node;

void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

Node* create_node(int key, int weight) {
    Node* node = malloc(sizeof(Node));
    node->key = key;
    node->weight = weight;
    node->left = node->right = NULL;
    return node;
}

void inorder_traversal(Node* root, int* result, int* index) {
    if (!root) return;
    inorder_traversal(root->left, result, index);
    result[(*index)++] = root->key;
    inorder_traversal(root->right, result, index);
}

void calculate_stats(Node* root, int depth, int* size, int* height, int* weighted_depth) {
    if (!root) return;
    (*size)++;
    *weighted_depth += root->weight * (depth + 1);
    int lh=0, rh=0, ls=0, rs=0, lw=0, rw=0;
    if (root->left)  calculate_stats(root->left, depth + 1, &ls, &lh, &lw);
    if (root->right) calculate_stats(root->right, depth + 1, &rs, &rh, &rw);
    *height = 1 + (lh > rh ? lh : rh);
    *size += ls + rs;
    *weighted_depth += lw + rw;
}

Node* right_rotate(Node* y) {
    if (!y || !y->left) return y;
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    return x;
}

Node* left_rotate(Node* x) {
    if (!x || !x->right) return x;
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node* insert_root(Node* tree, int key, int weight) {
    if (!tree) return create_node(key, weight);
    if (key < tree->key) {
        tree->left = insert_root(tree->left, key, weight);
        return right_rotate(tree);
    } else {
        tree->right = insert_root(tree->right, key, weight);
        return left_rotate(tree);
    }
}

Node* build_a2(int* keys, int* weights, int L, int R) {
    if (L > R) return NULL;
    int total = 0; for (int i=L;i<=R;i++) total += weights[i];
    int sum = 0, i=L;
    while (i <= R) {
        sum += weights[i];
        if (sum >= total / 2) break;
        i++;
    }
    if (i > R) i = R;
    Node* root = create_node(keys[i], weights[i]);
    root->left = build_a2(keys, weights, L, i - 1);
    root->right = build_a2(keys, weights, i + 1, R);
    return root;
}

Node* build_tree_rec(int** root_table, int* keys, int* weights, int i, int j) {
    if (i > j) return NULL;
    int r = root_table[i][j];
    Node* root = create_node(keys[r], weights[r]);
    root->left = build_tree_rec(root_table, keys, weights, i, r - 1);
    root->right = build_tree_rec(root_table, keys, weights, r + 1, j);
    return root;
}

Node* build_optimal_tree(int* keys, int* weights, int n) {
    int** dp = malloc(n * sizeof(int*));
    int** root = malloc(n * sizeof(int*));
    int* prefix = malloc((n + 1) * sizeof(int));
    for (int i=0;i<n;i++){
        dp[i] = calloc(n, sizeof(int));
        root[i] = calloc(n, sizeof(int));
    }
    prefix[0]=0; for(int i=0;i<n;i++) prefix[i+1]=prefix[i]+weights[i];
    for(int i=0;i<n;i++){dp[i][i]=weights[i]; root[i][i]=i;}
    for(int L=2;L<=n;L++)
        for(int i=0;i<=n-L;i++){
            int j=i+L-1; dp[i][j]=INT_MAX;
            int total=prefix[j+1]-prefix[i];
            for(int r=i;r<=j;r++){
                int lc=(r>i)?dp[i][r-1]:0;
                int rc=(r<j)?dp[r+1][j]:0;
                int cost=lc+rc+total;
                if(cost<dp[i][j]){dp[i][j]=cost; root[i][j]=r;}
            }
        }
    Node* res=build_tree_rec(root,keys,weights,0,n-1);
    for(int i=0;i<n;i++){free(dp[i]);free(root[i]);}
    free(dp);free(root);free(prefix);
    return res;
}

void free_tree(Node* r){ if(!r)return; free_tree(r->left); free_tree(r->right); free(r); }

void print_tree_stats(Node* t, const char* name){
    int size=0, height=0, wdepth=0;
    calculate_stats(t, 0, &size, &height, &wdepth);
    printf("%s\t%d\t%d\t\t%d\t%.2f\n", name, size, 5050, height, (double)wdepth);
}

int* weights_ptr;
int compare_weights(const void* a, const void* b){
    int i1=*(int*)a, i2=*(int*)b;
    return weights_ptr[i2] - weights_ptr[i1];
}

int main() {
    srand(time(NULL));
    const int n = 100;
    int keys[n], weights[n];

    for (int i = 0; i < n; i++) keys[i] = i + 1;
    shuffle(keys, n);

    for (int i = 0; i < n; i++) weights[i] = rand() % 1000 + 1;

    Node* opt_tree = build_optimal_tree(keys, weights, n);

    int indices[n];
    for (int i = 0; i < n; i++) indices[i] = i;
    weights_ptr = weights;
    qsort(indices, n, sizeof(int), compare_weights);

    Node* a1_tree = NULL;
    for (int i = 0; i < n; i++) {
        int idx = indices[i];
        a1_tree = insert_root(a1_tree, keys[idx], weights[idx]);
    }

    
    Node* a2_tree = build_a2(keys, weights, 0, n - 1);

    printf("\nn=100\tРазмер\tКонтр.Сумма\tВысота\tСредневзвеш.высота\n");
    print_tree_stats(opt_tree, "ДОП");
    print_tree_stats(a1_tree, "А1");
    print_tree_stats(a2_tree, "А2");

    printf("\nInorder:\n");
    int* inorder_result = malloc(n * sizeof(int));
    int idx = 0;

    inorder_traversal(a1_tree, inorder_result, &idx);
    printf("А1: ");
    for (int i = 0; i < 100; i++) printf("%d ", inorder_result[i]);
    printf("\n");

    idx = 0;
    inorder_traversal(a2_tree, inorder_result, &idx);
    printf("А2: ");
    for (int i = 0; i < 100; i++) printf("%d ", inorder_result[i]);
    printf("\n");

    free(inorder_result);
    free_tree(opt_tree);
    free_tree(a1_tree);
    free_tree(a2_tree);
    return 0;
}
