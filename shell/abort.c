#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Test: Triggering SIGABRT (Abort)\n");
    abort();
    return 0;
}