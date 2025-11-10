// shannon_strict.c — код Шеннона строго по псевдокоду (Windows-1251, а..я)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    unsigned char ch;   // символ
    double p;           // вероятность
} Sym;

/* ===== Алгоритм Шеннона строго по псевдокоду =====
   (переменные и структура циклов — как на картинке)
*/
void shannon_build_exact(const Sym *A, int n,
                         double P[], double Q[], int L[], int C[][64])
{
    P[0] = 0.0;
    Q[0] = 0.0;

    // DO ( i := 1, ..., n )
    for (int i = 1; i <= n; i++) {
        P[i] = A[i - 1].p;
        Q[i] = Q[i - 1] + P[i];
        L[i] = (int)ceil(-log2(P[i]));
    }
    // OD

    // DO ( i := 1, ..., n )
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= L[i]; j++) {
            Q[i - 1] = Q[i - 1] * 2;
            C[i][j] = (int)floor(Q[i - 1]);
            if (Q[i - 1] > 1.0)
                Q[i - 1] = Q[i - 1] - 1.0;
        }
    }
    // OD
}

/* ===== Простая пузырьковая сортировка ===== */
void sort_by_probability(Sym *A, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (A[j].p < A[j + 1].p) {
                Sym t = A[j];
                A[j] = A[j + 1];
                A[j + 1] = t;
            }
        }
    }
}

/* ===== Главная программа ===== */
int main(void) {
    FILE *f = fopen("sh.txt", "rb");
    if (!f) { printf("Файл text.txt не найден\n"); return 1; }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *text = malloc(size);
    fread(text, 1, size, f);
    fclose(f);

    // Подсчёт частот а..я (в Windows-1251 диапазон 224–255)
    unsigned long freq[32] = {0};
    for (long i = 0; i < size; i++) {
        unsigned char c = text[i];
        if (c >= 224 && c <= 255) freq[c - 224]++;
    }

    unsigned long total = 0;
    for (int i = 0; i < 32; i++) total += freq[i];

    Sym A[32];
    int n = 0;
    for (int i = 0; i < 32; i++) {
        if (freq[i] > 0) {
            A[n].ch = (unsigned char)(224 + i);
            A[n].p  = (double)freq[i] / total;
            n++;
        }
    }

    if (n == 0) { printf("Нет символов а..я в файле\n"); free(text); return 0; }

    sort_by_probability(A, n);

    double P[33], Q[33];
    int L[33];
    int C[33][64];

    shannon_build_exact(A, n, P, Q, L, C);

    printf("Код Шеннона (строго по псевдокоду)\n");
    printf("%-8s %-14s %-18s %-6s\n", "Символ", "Вероятность", "Код", "Длина");

    for (int i = 1; i <= n; i++) {
        char code[64];
        for (int j = 1; j <= L[i]; j++)
            code[j - 1] = (char)('0' + C[i][j]);
        code[L[i]] = '\0';
        printf("%-8c %-14.8f %-18s %-6d\n", A[i - 1].ch, P[i], code, L[i]);
    }

    double kraft = 0, H = 0, Lbar = 0;
    for (int i = 1; i <= n; i++) {
        kraft += pow(2, -L[i]);
        H += -P[i] * log2(P[i]);
        Lbar += P[i] * L[i];
    }

    printf("\n%-20s %-15s %-20s %-15s\n",
           "Крафт", "Энтропия", "Средняя длина", "Избыточность");
    printf("%-20.8f %-15.8f %-20.8f %-15.8f\n",
           kraft, H, Lbar, Lbar - H);

    free(text);
    return 0;
}
