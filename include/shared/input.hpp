#pragma once

#include "common/common_header.hpp"

namespace drop::shared
{
    struct ScreenSize
    {
        u32 width {0};
        u32 height {0};
    };

    extern ScreenSize g_screenSize;
}
