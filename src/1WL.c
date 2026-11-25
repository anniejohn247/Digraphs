// trying to translate the 1-WL algorithm from python
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME 64

typedef struct {
    char name[MAX_NAME];
    char feature[MAX_NAME];
    int *nbrs;
    int deg;
    int cap;
} Node;

static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static int find_node(const Node *nodes, int n, const char *name) {
    for (int i = 0; i < n; i++)
        if (strcmp(nodes[i].name, name) == 0) return i;
    return -1;
}

static void add_neighbour(Node *u, int v) {
    if (u->deg == u->cap) {
        u->cap = u->cap ? u->cap * 2 : 4;
        u->nbrs = realloc(u->nbrs, u->cap * sizeof(int));
        if (!u->nbrs) { perror("realloc"); exit(1); }
    }
    u->nbrs[u->deg++] = v;
}

/* Convert original features into integer colour IDs */
static int *initialise_colours(const Node *nodes, int n) {
    int *colours = malloc(n * sizeof(int));
    int *rep = malloc(n * sizeof(int));   /* representative node for each colour id */
    int num = 0;

    for (int i = 0; i < n; i++) {
        int id = -1;
        for (int c = 0; c < num; c++) {
            if (strcmp(nodes[rep[c]].feature, nodes[i].feature) == 0) { id = c; break; }
        }
        if (id < 0) { id = num; rep[num++] = i; }
        colours[i] = id;
    }
    free(rep);
    return colours;
}

/* Update colours using each node's colour and its neighbours' colours */
static int *refine_colours(const Node *nodes, int n, const int *colours) {
    /* signature[i] = [own colour, sorted neighbour colours...], length deg+1 */
    int **sig = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        int d = nodes[i].deg;
        sig[i] = malloc((d + 1) * sizeof(int));
        sig[i][0] = colours[i];
        for (int k = 0; k < d; k++)
            sig[i][k + 1] = colours[nodes[i].nbrs[k]];
        qsort(sig[i] + 1, d, sizeof(int), cmp_int);
    }

    /* Give identical signatures identical colour ids */
    int *new_colours = malloc(n * sizeof(int));
    int *rep = malloc(n * sizeof(int));
    int num = 0;

    for (int i = 0; i < n; i++) {
        int id = -1;
        for (int c = 0; c < num; c++) {
            int j = rep[c];
            if (nodes[j].deg == nodes[i].deg &&
                memcmp(sig[j], sig[i], (nodes[i].deg + 1) * sizeof(int)) == 0) {
                id = c; break;
            }
        }
        if (id < 0) { id = num; rep[num++] = i; }
        new_colours[i] = id;
    }

    for (int i = 0; i < n; i++) free(sig[i]);
    free(sig);
    free(rep);
    return new_colours;
}

static void print_colours(const Node *nodes, int n, const int *colours) {
    printf("{");
    for (int i = 0; i < n; i++)
        printf("%s%s: %d", i ? ", " : "", nodes[i].name, colours[i]);
    printf("}");
}

/* Run 1-WL colour refinement until convergence; returns final colours */
static int *wl_1(const Node *nodes, int n) {
    int *colours = initialise_colours(nodes, n);
    int iteration = 0;

    while (1) {
        printf("Iteration %d: ", iteration);
        print_colours(nodes, n, colours);
        printf("\n\n");

        int *new_colours = refine_colours(nodes, n, colours);

        if (memcmp(new_colours, colours, n * sizeof(int)) == 0) {
            free(new_colours);
            break;
        }
        free(colours);
        colours = new_colours;
        iteration++;
    }

    printf("Final colours: ");
    print_colours(nodes, n, colours);
    printf("\n");
    return colours;
}

int main(void) {
    int n, m;

    if (scanf("%d", &n) != 1 || n <= 0) { fprintf(stderr, "Bad node count\n"); return 1; }

    Node *nodes = calloc(n, sizeof(Node));
    for (int i = 0; i < n; i++) {
        if (scanf("%63s %63s", nodes[i].name, nodes[i].feature) != 2) {
            fprintf(stderr, "Bad node line\n"); return 1;
        }
    }


    if (scanf("%d", &m) != 1 || m < 0) { fprintf(stderr, "Bad edge count\n"); return 1; }

    for (int e = 0; e < m; e++) {
        char a[MAX_NAME], b[MAX_NAME];
        if (scanf("%63s %63s", a, b) != 2) { fprintf(stderr, "Bad edge line\n"); return 1; }
        int u = find_node(nodes, n, a), v = find_node(nodes, n, b);
        if (u < 0 || v < 0) { fprintf(stderr, "Unknown node in edge: %s %s\n", a, b); return 1; }
        add_neighbour(&nodes[u], v);
        add_neighbour(&nodes[v], u);
    }

    clock_t start = clock();

    int *colours = wl_1(nodes, n);

    /* Multiset of final colours (like Counter(colours.values())) */
    int *counts = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) counts[colours[i]]++;
    printf("Final multiset: {");
    int first = 1;
    for (int c = 0; c < n; c++) {
        if (counts[c]) {
            printf("%s%d: %d", first ? "" : ", ", c, counts[c]);
            first = 0;
        }
    }
    printf("}\n");

    clock_t end = clock();
    printf("Total runtime of program is %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < n; i++) free(nodes[i].nbrs);
    free(nodes);
    free(colours);
    free(counts);
    return 0;
}