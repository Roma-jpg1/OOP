#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct QueueNode {
    Node* treeNode;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue* q, Node* treeNode) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->treeNode = treeNode;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    
    q->rear->next = newNode;
    q->rear = newNode;
}

Node* dequeue(Queue* q) {
    if (q->front == NULL) {
        return NULL;
    }
    
    QueueNode* temp = q->front;
    Node* treeNode = temp->treeNode;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    return treeNode;
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node* build_complete_idx(int i, int n) {
    if (i > n) return NULL;
    Node* node = createNode(i);
    node->left = build_complete_idx(2*i, n);
    node->right = build_complete_idx(2*i+1, n);
    return node;
}

void inOrderTraversal(Node* root) {
    if (root == NULL) {
        return;
    }
    inOrderTraversal(root->left);
    printf("%d ", root->key);
    inOrderTraversal(root->right);
}

int treeSize(Node* root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + treeSize(root->left) + treeSize(root->right);
}

int checkSum(Node* root) {
    if (root == NULL) {
        return 0;
    }
    return root->key + checkSum(root->left) + checkSum(root->right);
}

int treeHeight(Node* root) {
    if (root == NULL) {
        return 0;
    }
    
    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);
    
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int sumHeights(Node* root, int depth) {
    if (root == NULL) {
        return 0;
    }
    return depth + sumHeights(root->left, depth + 1) + sumHeights(root->right, depth + 1);
}

int main() {
    Node* root = build_complete_idx(1, 100);

    printf("обход слева направо:\n");
    inOrderTraversal(root);
    printf("\n\n");

    int size = treeSize(root);
    int sum = checkSum(root);
    int height = treeHeight(root);
    int totalHeightSum = sumHeights(root, 1);
    double averageHeight = (double)totalHeightSum / size;

    printf("Tree characteristics:\n");
    printf("Size: %d\n", size);
    printf("Check sum: %d\n", sum);
    printf("Height: %d\n", height);
    printf("Average height: %.2f\n", averageHeight);
    
    return 0;
}