#include "mylib.h"
#include <stdio.h>
#include <string.h>

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

/* Returns 0.0 and prints an error if b is zero. */
double divide(double a, double b) {
    if (b == 0.0) {
        fprintf(stderr, "Error: division by zero\n");
        return 0.0;
    }
    return a / b;
}

void reverse_string(char *s) {
    int len = strlen(s);
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}

/* Iterative Fibonacci — avoids stack overflow for large n. */
long fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    long a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        long c = a + b;
        a = b;
        b = c;
    }
    return b;
}
