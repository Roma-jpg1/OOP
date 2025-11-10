#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

typedef struct Node {
    int key;
    int weight;
    struct Node *left;
    struct Node *right;
} Node;

void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);   
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

int weights[N];
int AW[N+1][N+1];
int AP[N+1][N+1];
int AR[N+1][N+1];

void PRI(int A[N+1][N+1], const char *name) {
    printf("\nМатрица %s:\n", name);
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            if (j > i)
                printf("%6d ", A[i][j]);
            else
                printf("   -   ");
        }
        printf("\n");
    }

    printf("\n\n\n\n");
}

void computeAW(int n) {
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= n; j++)
            AW[i][j] = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j <= n; j++) {
            if (j == i + 1)
                AW[i][j] = weights[i];
            else
                AW[i][j] = AW[i][j - 1] + weights[j - 1];
        }
    }
}

void computeAP_AR(int n) {
    int i, j, h, k, m, min, x;

    for (i = 0; i < n; i++) {
        j = i + 1;
        AP[i][j] = AW[i][j];
        AR[i][j] = j;
    }

    for (h = 2; h <= n; h++) {
        for (i = 0; i <= n - h; i++) {
            j = i + h;
            m = AR[i][j - 1];
            min = AP[i][m - 1] + AP[m][j];
            for (k = m + 1; k <= AR[i + 1][j]; k++) {
                x = AP[i][k - 1] + AP[k][j];
                if (x < min) {
                    min = x;
                    m = k;
                }
            }
            AP[i][j] = min + AW[i][j];
            AR[i][j] = m;
        }
    }
}



Node* buildTree(int L, int R, int a[]) {
    if (L >= R) return NULL;
    int k = AR[L][R];
    Node *node = (Node*)malloc(sizeof(Node));
    node->key = a[k - 1];
    node->weight = weights[k - 1];

    node->left  = buildTree(L, k - 1, a);
    node->right = buildTree(k, R, a);
    return node;
}


void inorder(Node *root) {
    if (!root) return;
    inorder(root->left);
    printf("%d(%d) ", root->key, root->weight);
    inorder(root->right);
}

void computeStats(Node *root, int depth,
                  int *pSize, int *pSumW,
                  int *pHeight, int *pWDepthSum)
{
    if (!root) {
        *pSize = 0; *pSumW = 0; *pHeight = 0; *pWDepthSum = 0;
        return;
    }
    int ls, rs, lw, rw, lh, rh, lwd, rwd;
    computeStats(root->left,  depth + 1, &ls, &lw, &lh, &lwd);
    computeStats(root->right, depth + 1, &rs, &rw, &rh, &rwd);

    *pSize = 1 + ls + rs;
    *pSumW = root->weight + lw + rw;
    *pHeight = 1 + (lh > rh ? lh : rh);
    *pWDepthSum = root->weight * depth + lwd + rwd;
}

int main(void) {
    srand((unsigned)time(NULL));

    int a[N];

    for (int i = 0; i < N; ++i) a[i] = i + 1;
    shuffle(a, N);

    for (int i = 0; i < N; i++)
        weights[i] = (rand() % 100) + 1;

    computeAW(N);
    computeAP_AR(N);
    Node *root = buildTree(0, N, a);

    PRI(AW, "AW ");
    PRI(AP, "AP ");
    
    PRI(AR, "AR ");

    printf("\n--- ОБХОД СЛЕВА НАПРАВО ---\n");
    inorder(root);
    printf("\n");

    int size, height;
    int sumW, wDepthSum;
    computeStats(root, 1, &size, &sumW, &height, &wDepthSum);
    double avgWHeight = (double)wDepthSum / (double)sumW;
    double checkRatio = (double)AP[0][N] / (double)AW[0][N];

    printf("\nТаблица:\n");
    printf("n=100   Размер   Контр.Сумма   Высота   Средневзвеш.высота\n");
    printf("ДОП     %d        %d           %d        %.6f\n",
           size, 5050, height, avgWHeight);

    printf("\nПроверка: AP[0][N]/AW[0][N] = %.6f\n", checkRatio);
    return 0;
}
