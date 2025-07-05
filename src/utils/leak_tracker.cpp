#include "utils/leak_tracker.hpp"

#include <mutex>
#include <vector>

// #ifdef _DEBUG
namespace
{
    struct Entry
    {
        void*       ptr;
        LeakType    type;
        const char* file;
        int         line;
        const char* desc;
        bool        freed {false};
    };

    std::vector<Entry> g_entries;
    std::mutex         g_mutex;

    const char* ToString(LeakType type)
    {
        switch (type)
        {
        case LeakType::HEAP:
            return "HEAP";
        case LeakType::OPENGL:
            return "OPENGL";
        case LeakType::HANDLE:
            return "HANDLE";
        case LeakType::CUSTOM:
            return "CUSTOM";
        default:
            return "UNKNOWN";
        }
    }

} // namespace anonymous

void _Register(void* ptr, LeakType type, const char* file, int line, const char* desc)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_entries.push_back({ptr, type, file, line, desc, false});
}

void _Unregister(void* ptr)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    for (auto& e : g_entries)
    {
        if (e.ptr == ptr && !e.freed)
        {
            e.freed = true;
            return;
        }
    }
    D_LEAK("Failed to unregister pointer: %p, it was not found or has already been freed.", ptr);
}

void _ReportLeaks()
{
    std::lock_guard<std::mutex> lock(g_mutex);

    bool leakFound {false};
    for (const auto& e : g_entries)
    {
        if (!e.freed)
        {
            leakFound = true;
            D_LEAK("Memory leak detected at pointer: %p, type: %s, file: %s, line: %d, desc: %s", e.ptr, ToString(e.type), e.file, e.line, e.desc);
        }
    }

    if (!leakFound)
    {
        D_LEAK("No memory leaks detected");
    }
}

// #endif // _DEBUG
