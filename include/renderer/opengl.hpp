#pragma once

#include "common/common_header.hpp"
#include "platform/window.hpp"
#include "utils/bump_allocator.hpp"

#include "opengl/glcorearb.h"
#include "opengl/glext.h"

namespace drop::renderer
{

    bool RendererInit(platform::WindowInfoPtr);
    bool RendererCreateContext(platform::WindowInfoPtr, utils::BumpAllocator* transientStorage);
    void RendererUpdateContext();
    void RendererDestroyContext();
    void RendererShutdown();
} // namespace drop::renderer