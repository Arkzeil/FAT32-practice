#include "allocator.h"
#include "utils.h"

// get the heap address
char *allocated = (char*)&__end;
int heap_offset = 0;

void* simple_malloc(unsigned int size) {
    size += align_offset(size, 8); // align to 8 bytes
    
    if(heap_offset + size > MAX_HEAP_SIZE)
        return 0; // out of memory
    
    allocated += size;
    heap_offset += size;
    
    return (void*)(allocated - size);
}