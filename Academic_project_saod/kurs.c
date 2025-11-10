#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>
#include <stdbool.h>

#define PAGE 20
#define MAX 4000


//CP866 → UTF-8

static void cp866_to_utf8(const char *src, char *dst, size_t cap) {
    size_t j = 0;

    for (size_t i = 0; src[i] && j + 2 < cap; i++) {
        unsigned char c = (unsigned char)src[i];

        if (c >= 0x80 && c <= 0xAF) {
            dst[j++] = 0xD0;
            dst[j++] = 0x90 + (c - 0x80);
        }
        else if (c >= 0xE0 && c <= 0xEF) {
            dst[j++] = 0xD1;
            dst[j++] = 0x80 + (c - 0xE0);
        }
        else if (c == 0xF0) {
            dst[j++] = 0xD0;
            dst[j++] = 0x81;
        }
        else if (c == 0xF1) {
            dst[j++] = 0xD1;
            dst[j++] = 0x91;
        }
        else dst[j++] = c;
    }

    dst[j] = 0;
}


static void str_prepare(const char *src, int len, char *dst, size_t cap) {
    char tmp[256];
    int n = len;

    while (n > 0 && src[n - 1] == 0) n--;
    if (n > (int)sizeof(tmp) - 1) n = sizeof(tmp) - 1;

    memcpy(tmp, src, n);
    tmp[n] = 0;

    for (int i = 0; i < n; i++) {
        if (tmp[i] == '_') tmp[i] = ' ';
    }

    cp866_to_utf8(tmp, dst, cap);
}


static void str_upper_ru(char *s) {
    for (size_t i = 0; s[i]; i++) {
        if ((unsigned char)s[i] == 0xD1 && s[i + 1] == (char)0x91) {
            s[i] = 0xD0;
            s[i + 1] = 0x81;
            i++;
        }
    }
}


static void surname_from_title(const char *title, char *out, size_t cap) {
    char buf[128];
    str_prepare(title, 32, buf, sizeof(buf));

    int end = (int)strlen(buf) - 1;
    while (end >= 0 && isspace((unsigned char)buf[end])) end--;

    int start = end;
    while (start >= 0 && !isspace((unsigned char)buf[start])) start--;

    start++;
    int len = end - start + 1;

    if (len < 0) len = 0;
    if (len >= (int)cap) len = (int)cap - 1;

    memcpy(out, buf + start, len);
    out[len] = 0;

    str_upper_ru(out);
}


typedef struct {
    char author[12];
    char title[32];
    char pub[16];
    short year;
    short pages;
} Rec;

typedef struct Node {
    Rec r;
    float w;
    struct Node *L, *R;
} Node;

typedef struct QNode {
    Rec data;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
    int size;
} Queue;


static void q_init(Queue *q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

static bool q_empty(const Queue *q) {
    return q->front == NULL;
}

static void q_push(Queue *q, Rec r) {
    QNode *n = malloc(sizeof(QNode));
    if (!n) {
        fprintf(stderr, "Ошибка: не хватает памяти под узел очереди\n");
        exit(1);
    }

    n->data = r;
    n->next = NULL;

    if (q->rear)
        q->rear->next = n;
    else
        q->front = n;

    q->rear = n;
    q->size++;
}

static Rec q_pop(Queue *q) {
    Rec e = {0};
    if (q_empty(q)) return e;

    QNode *tmp = q->front;
    e = tmp->data;
    q->front = tmp->next;

    if (!q->front)
        q->rear = NULL;

    free(tmp);
    q->size--;

    return e;
}

static int q_count(const Queue *q) { return q->size; }
static void q_clear(Queue *q) { while (!q_empty(q)) q_pop(q); }


static int rec_cmp(const Rec *a, const Rec *b) {
    char sa[64], sb[64];
    surname_from_title(a->title, sa, sizeof(sa));
    surname_from_title(b->title, sb, sizeof(sb));

    int c = strcmp(sa, sb);
    return c ? c : memcmp(a->title, b->title, 32);
}


static void sort_hoare(Rec *a, int L, int R) {
    if (L >= R) return;

    int i = L, j = R;
    Rec p = a[(L + R) / 2];

    while (i <= j) {
        while (rec_cmp(&a[i], &p) < 0) i++;
        while (rec_cmp(&a[j], &p) > 0) j--;

        if (i <= j) {
            Rec t = a[i];
            a[i] = a[j];
            a[j] = t;
            i++;
            j--;
        }
    }

    if (L < j) sort_hoare(a, L, j);
    if (i < R) sort_hoare(a, i, R);
}


static int search_pref(Rec *a, int n, const char *pref, Queue *q) {
    q_clear(q);
    if (!n || !*pref) return 0;

    int l = 0, r = n - 1, found = -1;
    size_t L = strlen(pref);

    while (l <= r) {
        int m = (l + r) / 2;
        char s[64];
        surname_from_title(a[m].title, s, sizeof(s));

        int cmp = strncmp(s, pref, L);

        if (cmp == 0) { found = m; break; }
        else if (cmp < 0) l = m + 1;
        else r = m - 1;
    }

    if (found < 0) return 0;

    int s = found, e = found;
    char tmp[64];

    while (s > 0) {
        surname_from_title(a[s - 1].title, tmp, sizeof(tmp));
        if (!strncmp(tmp, pref, L)) s--;
        else break;
    }

    while (e < n - 1) {
        surname_from_title(a[e + 1].title, tmp, sizeof(tmp));
        if (!strncmp(tmp, pref, L)) e++;
        else break;
    }

    for (int i = s; i <= e; i++)
        q_push(q, a[i]);

    return e - s + 1;
}


static void set_weights(Rec *a, float *w, int n) {
    if (!n) return;

    int min = a[0].year, max = a[0].year;

    for (int i = 1; i < n; i++) {
        if (a[i].year < min) min = a[i].year;
        if (a[i].year > max) max = a[i].year;
    }

    for (int i = 0; i < n; i++)
        w[i] = (float)(max - a[i].year + 1);
}

static Node *build_tree(Rec *a, float *w, int n) {
    if (!n) return NULL;

    int mx = 0;
    for (int i = 1; i < n; i++)
        if (w[i] > w[mx]) mx = i;

    Node *x = malloc(sizeof(Node));
    x->r = a[mx];
    x->w = w[mx];
    x->L = build_tree(a, w, mx);
    x->R = build_tree(a + mx + 1, w + mx + 1, n - mx - 1);
    return x;
}

static void show(Rec *a, const float *w, int n) {
    int pos = 0;

    while (pos < n) {
        printf("\n--------------------------------------------------------------------------------------------------------------------------------\n");
        printf("| %-3s | %-12s | %-32s | %-16s | %-4s | %-6s | %-5s |\n",
               "№", "Автор", "Заглавие", "Издательство", "Год", "Стр.", "Вес");
        printf("--------------------------------------------------------------------------------------------------------------------------------\n");

        for (int i = 0; i < PAGE && pos < n; i++, pos++) {
            char au[128], ti[128], pu[128];
            str_prepare(a[pos].author, 12, au, sizeof(au));
            str_prepare(a[pos].title, 32, ti, sizeof(ti));
            str_prepare(a[pos].pub, 16, pu, sizeof(pu));

            printf("| %-3d | %-12s | %-32s | %-16s | %4d | %6d | %5.0f |\n",
                   pos + 1, au, ti, pu, a[pos].year, a[pos].pages, w ? w[pos] : 0);
        }

        printf("\n");

        if (pos < n) {
            printf("\nПоказать следующие %d записей? (y/n): ", PAGE);
            int c = getchar();
            while (getchar() != '\n');
            if (tolower(c) != 'y') break;
        }
    }
}


int main(void) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    FILE *f = fopen("testBase1.dat", "rb");
    if (!f) {
        perror("testBase1.dat");
        return 1;
    }

    Rec db[MAX];
    int n = fread(db, sizeof(Rec), MAX, f);
    fclose(f);

    Rec sorted[MAX], out[MAX];
    float w[MAX], w2[MAX];

    Node *root = NULL;
    Queue q;
    q_init(&q);

    int choice;

    do {
        printf("\n=== МЕНЮ ===\n");
        printf("1. Показать базу\n");
        printf("2. Сортировать по фамилии\n");
        printf("3. Поиск по фамилии (с выводом)\n");
        printf("4. Построить дерево A2 по году\n");
        printf("5. Поиск по году в дереве\n");
        printf("0. Выход\n> ");

        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                show(db, NULL, n);
                break;

            case 2:
                memcpy(sorted, db, n * sizeof(Rec));
                sort_hoare(sorted, 0, n - 1);
                show(sorted, NULL, n);
                break;

            case 3: {
                memcpy(sorted, db, n * sizeof(Rec));
                sort_hoare(sorted, 0, n - 1);

                char pref[64];
                printf("Введите префикс фамилии: ");
                if (!fgets(pref, sizeof(pref), stdin)) break;

                pref[strcspn(pref, "\r\n")] = 0;
                str_upper_ru(pref);

                int found = search_pref(sorted, n, pref, &q);

                if (!found) {
                    printf("Ничего не найдено.\n");
                    break;
                }

                printf("\nНайдено %d записей по фамилии \"%s\":\n", found, pref);

                Rec *tmp = malloc(found * sizeof(Rec));
                QNode *p = q.front;

                for (int i = 0; i < found && p; i++, p = p->next)
                    tmp[i] = p->data;

                show(tmp, NULL, found);
                free(tmp);
                break;
            }

            case 4: {
                if (q_empty(&q)) {
                    printf("Очередь пуста. Сначала выполните поиск.\n");
                    break;
                }

                int size = q_count(&q);
                Rec *tmp = malloc(size * sizeof(Rec));
                float *w1 = malloc(size * sizeof(float));

                for (int i = 0; i < size; i++)
                    tmp[i] = q_pop(&q);

                set_weights(tmp, w1, size);
                root = build_tree(tmp, w1, size);

                int c = 0;
                inorder(root, out, w2, &c);

                printf("\n=== ДЕРЕВО A2 ===\n");
                show(out, w2, c);

                free(tmp);
                free(w1);
                break;
            }

            case 5: {
                if (!root) {
                    printf("Сначала постройте дерево (пункт 4)\n");
                    break;
                }

                int year;
                printf("Введите год: ");
                if (scanf("%d", &year) != 1) { while (getchar() != '\n'); break; }
                while (getchar() != '\n');

                Rec found[MAX];
                float wf[MAX];
                int c = 0;

                find_by_year(root, year, found, wf, &c);

                if (!c)
                    printf("Книги за %d год не найдены.\n", year);
                else {
                    printf("\nНайдено %d книг(и):\n", c);
                    show(found, wf, c);
                }

                break;
            }

            case 0:
                printf("Выход...\n");
                break;

            default:
                printf("Неверный пункт.\n");
                break;
        }

    } while (choice != 0);

    q_clear(&q);
    return 0;
}
