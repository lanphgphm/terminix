#include <stdio.h>

int main() {
    printf("Test 4: Triggering SIGILL (Illegal Instruction)\n");
    asm("ud2");  // Invalid opcode to raise SIGILL
    return 0;
}
