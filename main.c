#include <stdio.h>

char *strcpy_asm(char *restrict destination, char const *restrict source) {
    char *return_pointer = destination;
    #if defined (__aarch64__)
        char temporary = 0;
        __asm__ volatile (
            ".Loop:                         \n\t"
            "ldrb %w[tmp], [%x[src], #1]!   \n\t"    // load and post-increment
            "strb %w[tmp], [%x[dst], #1]!   \n\t"    // store and post-increment
            "cbnz %w[tmp], .Loop"
            : [dst] "+r"(destination),
              [src] "+r"(source),
              [tmp] "=&r"(temporary)
            :
            : "memory"
        );
    #elif defined (__x86_64__)
        char *temporary;
        long counter = -1;
        __asm__ volatile (
            "cld                            \n\t"   // set `rep` direction
            "movq %[dst], %[tmp]            \n\t"
            "movq %[src], %[dst]            \n\t"   // scas expects to work with rdi
            "repne scasb                    \n\t"
            "not %[ctr]                     \n\t"   // rcx now has the string length
            "movq %[tmp], %[dst]            \n\t"
            "rep movsb"
            : [dst] "+D"(destination),      // rdi
              [src] "+S"(source),           // rsi
              [ctr] "+c"(counter),          // rcx
              [tmp] "=&r"(temporary)
            : [acc] "a"((unsigned char)0)   // al
            : "cc", "memory"
        );
    #elif defined (__m68k__)
        __asm__ volatile (
            "1: move.b (%[dst])+,(%[src])+  \n\t"
            "   bne 1b                      \n\t"
            : [dst] "+a"(destination), [src] "+a"(source)
            :
            : "cc", "memory"
        );
#endif
    return return_pointer;
}

char *strcpy_c(char *restrict destination, char const *restrict source) {
    char *return_pointer = destination;
    while ((*destination++ = *source++)) {}
    return return_pointer;
}


int main(void) {
    printf("Hello, World!\n");
    return 0;
}
