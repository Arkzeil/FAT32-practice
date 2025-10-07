#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MAX_HEAP_SIZE       4194304 // 4MB
// Get the symbol __end from linker script
extern char* __end;
extern char* _start;
// make allocated variable global among all files
extern char* allocated;
extern int heap_offset;

// return requested 'size' bytes which are continuous space
void* simple_malloc(unsigned int size);

#endif // ALLOCATOR_H