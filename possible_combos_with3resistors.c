#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_RESISTORS 50
#define MAX_RESULTS   10000
#define TOLERANCE     1e-2   // tolerance for deduplication

typedef struct {
    double value;
    char expr[128];
} Result;

Result results[MAX_RESULTS];
int resultCount = 0;

double series(double r1, double r2) { return r1 + r2; }
double parallel(double r1, double r2) { return 1.0 / (1.0/r1 + 1.0/r2); }

// check if value already exists (within tolerance)
int exists(double val) {
    for (int i = 0; i < resultCount; i++) {
        if (fabs(results[i].value - val) < TOLERANCE) return 1;
    }
    return 0;
}

// add new result if unique
void addResult(double val, const char *expr) {
    if (!exists(val) && resultCount < MAX_RESULTS) {
        results[resultCount].value = val;
        strncpy(results[resultCount].expr, expr, sizeof(results[resultCount].expr)-1);
        results[resultCount].expr[sizeof(results[resultCount].expr)-1] = '\0';
        resultCount++;
    }
}

// recursive for 3 resistors
void recurse(double resistors[], char *exprs[], int count) {
    if (count == 1) {
        addResult(resistors[0], exprs[0]);
        return;
    }
    for (int i = 0; i < count; i++) {
        for (int j = i+1; j < count; j++) {
            double newArr[count-1];
            char *newExpr[count-1];
            int idx = 0;
            for (int k = 0; k < count; k++) {
                if (k != i && k != j) {
                    newArr[idx] = resistors[k];
                    newExpr[idx] = exprs[k];
                    idx++;
                }
            }
            // series
            double rS = series(resistors[i], resistors[j]);
            char bufS[128];
            snprintf(bufS, sizeof(bufS), "(%s + %s)", exprs[i], exprs[j]);
            newArr[idx] = rS;
            newExpr[idx] = strdup(bufS);
            recurse(newArr, newExpr, count-1);

            // parallel
            double rP = parallel(resistors[i], resistors[j]);
            char bufP[128];
            snprintf(bufP, sizeof(bufP), "(%s || %s)", exprs[i], exprs[j]);
            newArr[idx] = rP;
            newExpr[idx] = strdup(bufP);
            recurse(newArr, newExpr, count-1);
        }
    }
}

int cmpResults(const void *a, const void *b) {
    double va = ((Result*)a)->value;
    double vb = ((Result*)b)->value;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

int main() {
    int N;
    double pool[MAX_RESISTORS];

    printf("Enter number of resistors in pool: ");
    scanf("%d", &N);
    if (N > MAX_RESISTORS) {
        printf("Too many resistors (max %d)\n", MAX_RESISTORS);
        return 1;
    }

    printf("Enter resistor values (ohms):\n");
    for (int i = 0; i < N; i++) {
        scanf("%lf", &pool[i]);
    }

    // 1 resistor
    for (int i = 0; i < N; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f", pool[i]);
        addResult(pool[i], buf);
    }

    // 2 resistors
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            double r1 = pool[i], r2 = pool[j];
            char bufS[64], bufP[64];
            snprintf(bufS, sizeof(bufS), "(%.0f + %.0f)", r1, r2);
            snprintf(bufP, sizeof(bufP), "(%.0f || %.0f)", r1, r2);
            addResult(series(r1,r2), bufS);
            addResult(parallel(r1,r2), bufP);
        }
    }

    // 3 resistors
    for (int a = 0; a < N; a++) {
        for (int b = a+1; b < N; b++) {
            for (int c = b+1; c < N; c++) {
                double combo[3] = {pool[a], pool[b], pool[c]};
                char *exprs[3];
                char bufA[16], bufB[16], bufC[16];
                snprintf(bufA, sizeof(bufA), "%.0f", pool[a]);
                snprintf(bufB, sizeof(bufB), "%.0f", pool[b]);
                snprintf(bufC, sizeof(bufC), "%.0f", pool[c]);
                exprs[0] = strdup(bufA);
                exprs[1] = strdup(bufB);
                exprs[2] = strdup(bufC);
                recurse(combo, exprs, 3);
            }
        }
    }

    // sort results
    qsort(results, resultCount, sizeof(Result), cmpResults);

    // print unique sorted results
    printf("\n--- Unique Results Sorted ---\n");
    for (int i = 0; i < resultCount; i++) {
        printf("%.2f Ω  ->  %s\n", results[i].value, results[i].expr);
    }

    return 0;
}
