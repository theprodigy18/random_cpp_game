#pragma once

#include "common/common_header.hpp"

enum class LeakType
{
    HEAP,
    OPENGL,
    HANDLE, 
    CUSTOM
};

// #ifdef _DEBUG

void _Register(void* ptr, LeakType type, const char* file, int line, const char* desc = {});
void _Unregister(void* ptr);
void _ReportLeaks();

#ifdef D_DEBUG
#define TRACK_LEAK_ALLOC(ptr, type, desc) _Register(ptr, type, __FILE__, __LINE__, desc)
#define TRACK_LEAK_FREE(ptr) _Unregister(ptr)
#define TRACK_LEAK_REPORT() _ReportLeaks()
#else
#define TRACK_LEAK_ALLOC(ptr, type, desc)
#define TRACK_LEAK_FREE(ptr)
#define TRACK_LEAK_REPORT()
#endif // D_DEBUG
