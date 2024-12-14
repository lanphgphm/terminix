#include <stdio.h>

int main() {
    printf("Test: Triggering SIGFPE (Floating-point Exception)\n");
    int a = 1;
    int b = 0;
    int result = a / b;  
    return 0;
}
