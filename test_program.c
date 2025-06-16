#include <stdio.h>
#include <stdlib.h>

int add_numbers(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    int result = add_numbers(x, y);
    
    printf("Hello from test program!\n");
    printf("The sum of %d and %d is %d\n", x, y, result);
    
    for (int i = 0; i < 5; i++) {
        printf("Loop iteration: %d\n", i);
    }
    
    return 0;
} 