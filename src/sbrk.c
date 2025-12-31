// Defined by the linker script
extern char _heap_start;

void* _sbrk(int increment) {
    static char* heap_end = &_heap_start;
    char* previous_heap_end = heap_end;
    heap_end += increment;
    return previous_heap_end;
}
