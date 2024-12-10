#include <stdio.h>

int main() {
    printf("Test: Triggering SIGSEGV (Segmentation Fault)\n");
    int *ptr = NULL;
    *ptr = 42; 
    return 0;
}
