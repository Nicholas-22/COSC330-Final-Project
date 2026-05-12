#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"

static void print_menu(void) {
    printf("\n===== My Math & Utility Shell =====\n");
    printf("1. Add two numbers\n");
    printf("2. Subtract two numbers\n");
    printf("3. Divide two numbers\n");
    printf("4. Reverse a string\n");
    printf("5. Fibonacci number\n");
    printf("6. Exit\n");
    printf("Choose an option: ");
}

static void get_two_doubles(double *a, double *b) {
    printf("Enter first number:  ");
    scanf("%lf", a);
    printf("Enter second number: ");
    scanf("%lf", b);
}

int main(void) {
    int choice;

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            /* flush bad input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        double a, b;
        char   buf[256];

        switch (choice) {
        case 1:
            get_two_doubles(&a, &b);
            printf("Result: %.6g\n", add(a, b));
            break;

        case 2:
            get_two_doubles(&a, &b);
            printf("Result: %.6g\n", subtract(a, b));
            break;

        case 3:
            get_two_doubles(&a, &b);
            printf("Result: %.6g\n", divide(a, b));
            break;

        case 4:
            printf("Enter a string: ");
            scanf("%255s", buf);
            reverse_string(buf);
            printf("Reversed: %s\n", buf);
            break;

        case 5: {
            int n;
            printf("Enter n: ");
            scanf("%d", &n);
            printf("fibonacci(%d) = %ld\n", n, fibonacci(n));
            break;
        }

        case 6:
            printf("Goodbye!\n");
            return 0;

        default:
            printf("Unknown option. Try again.\n");
        }
    }
}
