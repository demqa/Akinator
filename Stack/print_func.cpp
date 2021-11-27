#include <stdio.h>

void PrintString(void *memory, size_t size, FILE *stream){
    fprintf(stream, "{%s}\n", (char *)memory);
}