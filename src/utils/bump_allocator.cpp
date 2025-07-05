#include "utils/bump_allocator.hpp"

#include <cstdlib> // malloc.
#include <cstring> // memset.

// C-style managed memory allocator.
// I just want to keep this since i want to learn a lot about memory management.
// So that's why i decided to use this manual C-style allocator and not use any library from C++.

namespace drop::utils
{
    BumpAllocator MakeBumpAllocator(Size size)
    {
        BumpAllocator ba {};

        ba.memory = (Char*) malloc(size);
        if (ba.memory)
        {
            ba.capacity = size;
            memset(ba.memory, 0, size);
        }
        else
        {
            D_ASSERT(false, "Failed to allocate memory for bump allocator.");
        }

        return ba;
    }

    Char* BumpAlloc(BumpAllocator* ba, Size size)
    {
        Char* result {nullptr};

        Size allignedSize {(size + 15) & ~15}; // Allign to 16 bytes.
        if (ba->used + allignedSize <= ba->capacity)
        {
            result = ba->memory + ba->used;
            ba->used += allignedSize;
        }
        else
        {
            D_ASSERT(false, "Failed to allocate memory for bump allocator.");
        }

        return result;
    }
} // namespace drop::utils
