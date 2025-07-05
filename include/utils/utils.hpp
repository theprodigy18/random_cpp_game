#pragma once

#ifdef _WIN32
#define NATIVE_CHAR(c) L##c
#elif defined(__linux__)
#define NATIVE_CHAR(c) c
#endif // _WIN32

#define BIT(x) (1 << x)
#define KB(x) ((unsigned long long) 1024 * x)
#define MB(x) ((unsigned long long) 1024 * KB(x))
#define GB(x) ((unsigned long long) 1024 * MB(x))