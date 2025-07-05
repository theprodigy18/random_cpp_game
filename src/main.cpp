#include "renderer/opengl.hpp"

using namespace drop;

int main()
{
#ifdef _WIN32
    // This is just to make the window size not scaled. So it will be pixel perfect.
    SetProcessDPIAware();
#endif // DEBUG

    D_TRACE("Starting Drop Engine!");

    // Initialize platform and renderer.
    {
        if (!platform::PlatformInit())
        {
            D_ASSERT(false, "Failed to initialize platform!");
            return -1;
        }

        platform::WindowInfoPtr windowInfo {platform::PlatformCreateDummyWindow()};
        if (!windowInfo)
        {
            D_ASSERT(false, "Failed to create dummy window!");
            return -1;
        }

        if (!renderer::RendererInit(windowInfo))
        {
            D_ASSERT(false, "Failed to initialize renderer!");
            return -1;
        }

        platform::PlatformDestroyDummyWindow();
    }

    // Creating Window and Context.
    platform::WindowInfoPtr windowInfo {platform::PlatformCreateWindow(1280, 720, NATIVE_CHAR("Drop Engine"))};
    if (!windowInfo)
    {
        D_ASSERT(false, "Failed to create window!");
        return -1;
    }

    utils::BumpAllocator transientStorage {utils::MakeBumpAllocator(MB(50))};

    if (!renderer::RendererCreateContext(windowInfo, &transientStorage))
    {
        D_ASSERT(false, "Failed to create renderer context!");
        return -1;
    }

    // Main loop.
    bool running {true};
    while (running)
    {
        platform::PlatformUpdateWindow(running);
        renderer::RendererUpdateContext();
    }

    // Destroying Window and Context.
    renderer::RendererDestroyContext();
    platform::PlatformDestroyWindow();

    // Shutdown renderer and platform.
    renderer::RendererShutdown();
    platform::PlatformShutdown();

    TRACK_LEAK_REPORT();
    return 0;
}