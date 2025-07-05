#include "utils/file_io.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>

namespace drop::utils
{
    i64 GetTimestamp(Char* file)
    {
        struct stat fileStat {};
        stat(file, &fileStat);
        return fileStat.st_mtime;
    }

    bool FileExists(Char* filePath)
    {
        D_ASSERT(filePath, "File path is null.");

        std::ifstream file(filePath);
        return file.good();
    }

    i32 GetFileSize(Char* filePath)
    {
        D_ASSERT(filePath, "File path is null.");

        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", filePath);
            return 0;
        }

        return file.tellg();
    }

    // Reads a file into a buffer.
    Char* ReadFile(Char* filePath, Char* buffer, i32* outSize)
    {
        D_ASSERT(filePath, "File path is null.");
        D_ASSERT(outSize, "Size pointer is null.");
        D_ASSERT(buffer, "Buffer is null.");

        *outSize = 0;
        std::ifstream file(filePath, std::ios::binary);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", filePath);
            return nullptr;
        }

        file.seekg(0, std::ios::end);
        *outSize = file.tellg();
        file.seekg(0, std::ios::beg);

        file.read(buffer, *outSize);
        buffer[*outSize] = '\0'; // Add null terminator.

        return buffer;
    }

    Char* ReadFile(Char* filePath, BumpAllocator* ba, i32* outSize)
    {
        Char* file {nullptr};
        i32   fileSize {GetFileSize(filePath)};

        if (fileSize)
        {
            Char* buffer {BumpAlloc(ba, fileSize + 1)};
            file = ReadFile(filePath, buffer, outSize);
        }

        return file;
    }

    void WriteFile(Char* filePath, Char* buffer, i32 size)
    {
        D_ASSERT(filePath, "File path is null.");
        D_ASSERT(buffer, "Buffer is null.");

        std::ofstream file(filePath, std::ios::binary);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", filePath);
            return;
        }

        file.write(buffer, size);
    }

    bool CopyFile(Char* fileName, Char* destName, Char* buffer)
    {
        i32   fileSize {0};
        Char* data {ReadFile(fileName, buffer, &fileSize)};

        std::ofstream file(destName, std::ios::binary);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", destName);
            return false;
        }

        file.write(data, fileSize);
        return true;
    }

    bool CopyFile(Char* fileName, Char* destName, BumpAllocator* ba)
    {
        i32 fileSize {GetFileSize(fileName)};

        if (fileSize)
        {
            Char* buffer {BumpAlloc(ba, fileSize + 1)};
            return CopyFile(fileName, destName, buffer);
        }

        return false;
    }

} // namespace drop::utils