#pragma once

#include "common/common_header.hpp"
#include "bump_allocator.hpp"

namespace drop::utils
{
    i64   GetTimestamp(Char* file);
    bool  FileExists(Char* filePath);
    i32   GetFileSize(Char* filePath);
    Char* ReadFile(Char* filePath, Char* buffer, i32* outSize);
    Char* ReadFile(Char* filePath, BumpAllocator* ba, i32* outSize);
    void  WriteFile(Char* filePath, Char* buffer, i32 size);
    bool  CopyFile(Char* fileName, Char* destName, Char* buffer);
    bool  CopyFile(Char* fileName, Char* destName, BumpAllocator* ba);
} // namespace drop::utils
