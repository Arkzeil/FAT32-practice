#ifndef GPU_H
#define GPU_H

#define GPU_BASE   0xC0000000   // GPU base address
#define GPU_MEM    (GPU_BASE + 0x00000000) // GPU memory base
// Convert a physical address to a bus address
#define BUS_ADDRESS(phys)	(((phys) & ~0xC0000000) | GPU_MEM)

#endif // GPU_H