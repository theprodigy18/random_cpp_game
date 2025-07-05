#pragma once

#include "common/common_header.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif // _WIN32

namespace drop::platform
{
#ifdef _WIN32
    using TITLE = const WChar*;
#elif __linux__
    using TITLE = const Char*;
#endif // _WIN32

    struct WindowInfo
    {
#ifdef _WIN32
        HWND* hwnd;
        HDC*  hdc;
#elif defined(__linux__)
        Display**    display;
        ::Window*    window;
        GLXFBConfig* fbc;
#endif // _WIN32
    };

    using WindowInfoPtr = WindowInfo*;

    bool          PlatformInit();
    WindowInfoPtr PlatformCreateDummyWindow();
    void          PlatformDestroyDummyWindow();
    WindowInfoPtr PlatformCreateWindow(i32 width, i32 height, TITLE title);
    void          PlatformUpdateWindow(bool& running);
    void          PlatformDestroyWindow();
    void          PlatformShutdown();
} // namespace drop::platform