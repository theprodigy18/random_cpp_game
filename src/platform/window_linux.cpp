#include "platform/window.hpp"
#include "shared/input.hpp"

namespace drop::platform
{
    namespace
    {
        enum CallbackResult
        {
            NONE,
            QUIT
        };

        Display*      g_display {nullptr};
        ::Window      g_window {0};
        Atom          g_wmDeleteAtom {0};
        GLXFBConfig*  g_fbc {nullptr};
        GLXFBConfig   g_bestConfig {nullptr};
        Colormap      g_cmap {0};
        WindowInfoPtr g_windowInfo {nullptr};

        void CalculateCenterPosition(i32 screen, i32 width, i32 height, i32& outX, i32& outY)
        {
            Screen* screenInfo {XScreenOfDisplay(g_display, screen)};
            i32     screenWidth {screenInfo->width};
            i32     screenHeight {screenInfo->height};

            outX = (screenWidth - width) / 2;
            outY = (screenHeight - height) / 2;
        }

        CallbackResult CallbackHandle(XEvent& event)
        {
            switch (event.type)
            {
            case ClientMessage:
            {
                if ((Atom) event.xclient.data.l[0] == g_wmDeleteAtom)
                {
                    return QUIT;
                }
                break;
            }
            case ConfigureNotify:
            {
                shared::g_screenSize.width  = event.xconfigure.width;
                shared::g_screenSize.height = event.xconfigure.height;
                break;
            }
            default:
                break;
            }

            return NONE;
        }
    } // namespace anonymous

    bool PlatformInit()
    {
        if (!XInitThreads())
        {
            D_ASSERT(false, "Failed to initialize X11 threads");
            return false;
        }

        g_display = XOpenDisplay(nullptr);
        if (!g_display)
        {
            D_ASSERT(false, "Failed to open X11 display");
            return false;
        }
        TRACK_LEAK_ALLOC(g_display, LeakType::HANDLE, "X11 display");

        g_wmDeleteAtom = XInternAtom(g_display, "WM_DELETE_WINDOW", false);
        if (!g_wmDeleteAtom)
        {
            D_ASSERT(false, "Failed to get WM_DELETE_WINDOW atom");
            return false;
        }

        g_windowInfo = new WindowInfo {&g_display, &g_window, &g_bestConfig};
        TRACK_LEAK_ALLOC(g_windowInfo, LeakType::CUSTOM, "WindowInfo");

        return true;
    }

    WindowInfoPtr PlatformCreateDummyWindow()
    {
        i32      screen {XDefaultScreen(g_display)};
        ::Window root {XRootWindow(g_display, screen)};

        g_window = XCreateSimpleWindow(
            g_display,
            root,
            0,
            0,
            50,
            50,
            1,
            XWhitePixel(g_display, screen),
            XBlackPixel(g_display, screen));
        if (!g_window)
        {
            D_ASSERT(false, "Failed to create X11 window");
            return nullptr;
        }
        TRACK_LEAK_ALLOC((void*) g_window, LeakType::HANDLE, "X11 dummy window");

        return g_windowInfo;
    }

    void PlatformDestroyDummyWindow()
    {
        XDestroyWindow(g_display, g_window);
        TRACK_LEAK_FREE((void*) g_window);
        g_window = 0;
    }

    WindowInfoPtr PlatformCreateWindow(i32 width, i32 height, TITLE title)
    {
        i32      screen {XDefaultScreen(g_display)};
        ::Window root {XRootWindow(g_display, screen)};

        const i32 visualAttribs[] {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, True,
            None};

        i32 fbcount {0};
        g_fbc = glXChooseFBConfig(g_display, screen, visualAttribs, &fbcount);
        if (!g_fbc || !fbcount)
        {
            D_ASSERT(false, "Failed to choose X11 framebuffer config");
            return nullptr;
        }
        TRACK_LEAK_ALLOC(g_fbc, LeakType::HANDLE, "X11 framebuffer config");

        g_bestConfig = g_fbc[0];

        XVisualInfo* vi {glXGetVisualFromFBConfig(g_display, g_bestConfig)};
        if (!vi)
        {
            D_ASSERT(false, "Failed to get visual info");
            return nullptr;
        }
        TRACK_LEAK_ALLOC(vi, LeakType::HANDLE, "X11 visual info");

        g_cmap = XCreateColormap(g_display, root, vi->visual, AllocNone);
        TRACK_LEAK_ALLOC((void*) g_cmap, LeakType::HANDLE, "X11 colormap");

        XSetWindowAttributes attrs {};
        attrs.colormap   = g_cmap;
        attrs.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

        i32 x {0}, y {0};
        CalculateCenterPosition(screen, width, height, x, y);

        g_window = XCreateWindow(
            g_display,
            root,
            x,
            y,
            width,
            height,
            1,
            vi->depth,
            InputOutput,
            vi->visual,
            CWColormap | CWEventMask,
            &attrs);
        if (!g_window)
        {
            D_ASSERT(false, "Failed to create X11 window");
            return nullptr;
        }
        TRACK_LEAK_ALLOC((void*) g_window, LeakType::HANDLE, "X11 window");

        XStoreName(g_display, g_window, title);
        g_wmDeleteAtom = XInternAtom(g_display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(g_display, g_window, &g_wmDeleteAtom, 1);
        XMapWindow(g_display, g_window);
        XFlush(g_display);
        XFree(vi);
        TRACK_LEAK_FREE(vi);

        return g_windowInfo;
    }

    void PlatformUpdateWindow(bool& running)
    {
        while (XPending(g_display))
        {
            XEvent event {};
            XNextEvent(g_display, &event);
            CallbackResult result {CallbackHandle(event)};
            if (result == QUIT)
            {
                running = false;
                break;
            }
        }
    }

    void PlatformDestroyWindow()
    {
        XFreeColormap(g_display, g_cmap);
        TRACK_LEAK_FREE((void*) g_cmap);
        XDestroyWindow(g_display, g_window);
        TRACK_LEAK_FREE((void*) g_window);
        XFree(g_fbc);
        TRACK_LEAK_FREE(g_fbc);

        g_cmap       = 0;
        g_window     = 0;
        g_fbc        = nullptr;
        g_bestConfig = nullptr;
    }

    void PlatformShutdown()
    {
        XCloseDisplay(g_display);
        TRACK_LEAK_FREE(g_display);
        g_display = nullptr;

        delete g_windowInfo;
        TRACK_LEAK_FREE(g_windowInfo);
        g_windowInfo   = nullptr;
        g_wmDeleteAtom = 0;
    }

} // namespace drop::platform