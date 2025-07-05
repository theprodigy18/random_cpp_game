#include "platform/window.hpp"
#include "shared/input.hpp"

namespace drop::platform
{
    namespace
    {

        TITLE         g_className {nullptr};
        HWND          g_hwnd {nullptr};
        HDC           g_hdc {nullptr};
        WindowInfoPtr g_windowInfo {nullptr};
        bool          g_running {true};

        void CalculateCenterPosition(const RECT& rc, i32& outX, i32& outY)
        {
            RECT dr {};
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &dr, 0);

            i32 dw {dr.right - dr.left};
            i32 dh {dr.bottom - dr.top};

            i32 ww {rc.right - rc.left};
            i32 wh {rc.bottom - rc.top};

            outX = dr.left + (dw - ww) / 2;
            outY = dr.top + (dh - wh) / 2;
        }

        LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch (msg)
            {
            case WM_CLOSE:
            {
                PostQuitMessage(0);
                g_running = false;
                return 0;
            }
            case WM_SIZE:
            {
                RECT rc {};
                GetClientRect(hwnd, &rc);
                shared::g_screenSize.width  = rc.right - rc.left;
                shared::g_screenSize.height = rc.bottom - rc.top;

                break;
            }
            default:
                break;
            }

            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

    } // namespace anonymous

    bool PlatformInit()
    {
        g_windowInfo = new WindowInfo {&g_hwnd, &g_hdc};
        TRACK_LEAK_ALLOC(g_windowInfo, LeakType::CUSTOM, "WindowInfo");

        return true;
    }

    WindowInfoPtr PlatformCreateDummyWindow()
    {
        HINSTANCE hInstance {GetModuleHandleW(nullptr)};
        if (!hInstance)
        {
            D_ASSERT(false, "Failed to get module handle");
            return nullptr;
        }

        g_hwnd = CreateWindowExW(
            0,
            L"Static",
            L"Dummy",
            WS_OVERLAPPEDWINDOW,
            0, 0,
            50, 50,
            nullptr,
            nullptr,
            hInstance,
            nullptr);
        if (!g_hwnd)
        {
            D_ASSERT(false, "Failed to create window");
            return nullptr;
        }
        TRACK_LEAK_ALLOC(g_hwnd, LeakType::HANDLE, "Dummy HWND");

        g_hdc = GetDC(g_hwnd);
        if (!g_hdc)
        {
            D_ASSERT(false, "Failed to get device context");
            return nullptr;
        }
        TRACK_LEAK_ALLOC(g_hdc, LeakType::HANDLE, "Dummy HDC");

        return g_windowInfo;
    }

    void PlatformDestroyDummyWindow()
    {
        ReleaseDC(g_hwnd, g_hdc);
        TRACK_LEAK_FREE(g_hdc);
        DestroyWindow(g_hwnd);
        TRACK_LEAK_FREE(g_hwnd);

        g_hwnd = nullptr;
        g_hdc  = nullptr;
    }

    WindowInfoPtr PlatformCreateWindow(i32 width, i32 height, TITLE title)
    {
        HINSTANCE hInstance {GetModuleHandleW(nullptr)};
        if (!hInstance)
        {
            D_ASSERT(false, "Failed to get module handle");
            return nullptr;
        }

        WNDCLASSEXW wcex {};
        wcex.cbSize        = sizeof(WNDCLASSEXW);
        wcex.hInstance     = hInstance;
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wcex.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.hIconSm       = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.cbWndExtra    = 0;
        wcex.cbClsExtra    = 0;
        wcex.lpszClassName = title;
        wcex.lpszMenuName  = nullptr;
        wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
        wcex.lpfnWndProc   = WndProc;

        if (!RegisterClassExW(&wcex))
        {
            D_ASSERT(false, "Failed to register window class");
            return nullptr;
        }

        i32 dwStyle {WS_OVERLAPPEDWINDOW};

        RECT rc {0, 0, width, height};
        AdjustWindowRectEx(&rc, dwStyle, false, 0);

        i32 x {0}, y {0};
        CalculateCenterPosition(rc, x, y);

        g_hwnd = CreateWindowExW(
            0,
            title, title,
            dwStyle,
            x, y,
            rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr, hInstance, nullptr);

        if (!g_hwnd)
        {
            D_ASSERT(false, "Failed to create window");
            return nullptr;
        }
        TRACK_LEAK_ALLOC(g_hwnd, LeakType::HANDLE, "HWND");

        g_hdc = GetDC(g_hwnd);
        TRACK_LEAK_ALLOC(g_hdc, LeakType::HANDLE, "HDC");

        ShowWindow(g_hwnd, SW_SHOW);
        UpdateWindow(g_hwnd);

        g_className = title;

        shared::g_screenSize.width  = width;
        shared::g_screenSize.height = height;

        return g_windowInfo;
    }

    void PlatformUpdateWindow(bool& running)
    {
        MSG msg {};
        while (PeekMessageW(&msg, g_hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            if (msg.message == WM_QUIT)
            {
                break;
            }
        }

        running = g_running;
    }

    void PlatformDestroyWindow()
    {
        ReleaseDC(g_hwnd, g_hdc);
        TRACK_LEAK_FREE(g_hdc);
        DestroyWindow(g_hwnd);
        TRACK_LEAK_FREE(g_hwnd);

        g_hwnd = nullptr;
        g_hdc  = nullptr;

        UnregisterClassW(g_className, GetModuleHandleW(nullptr));
        g_className = nullptr;
    }

    void PlatformShutdown()
    {
        delete g_windowInfo;
        TRACK_LEAK_FREE(g_windowInfo);
        g_windowInfo = nullptr;
    }

} // namespace drop::platform