#include "renderer/opengl.hpp"
#include "utils/file_io.hpp"
#include "shared/input.hpp"

#include "opengl/glxext.h"
#include <GL/gl.h>

namespace drop::renderer
{

    namespace
    {
#define LOAD_GL_FUNCTION(type, name)                         \
    name = (type) glXGetProcAddress((const GLubyte*) #name); \
    D_ASSERT(name, "Failed to load OpenGL function: %s", #name);

#pragma region OpenGL Functions
        static PFNGLCREATEPROGRAMPROC           glCreateProgram;
        static PFNGLCREATESHADERPROC            glCreateShader;
        static PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
        static PFNGLUNIFORM1FPROC               glUniform1f;
        static PFNGLUNIFORM2FVPROC              glUniform2fv;
        static PFNGLUNIFORM3FVPROC              glUniform3fv;
        static PFNGLUNIFORM1IPROC               glUniform1i;
        static PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
        static PFNGLVERTEXATTRIBDIVISORPROC     glVertexAttribDivisor;
        static PFNGLACTIVETEXTUREPROC           glActiveTexture;
        static PFNGLBUFFERSUBDATAPROC           glBufferSubData;
        static PFNGLDRAWARRAYSINSTANCEDPROC     glDrawArraysInstanced;
        static PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
        static PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
        static PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
        static PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
        static PFNGLDRAWBUFFERSPROC             glDrawBuffers;
        static PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
        static PFNGLBLENDFUNCIPROC              glBlendFunci;
        static PFNGLBLENDEQUATIONPROC           glBlendEquation;
        static PFNGLCLEARBUFFERFVPROC           glClearBufferfv;
        static PFNGLSHADERSOURCEPROC            glShaderSource;
        static PFNGLCOMPILESHADERPROC           glCompileShader;
        static PFNGLGETSHADERIVPROC             glGetShaderiv;
        static PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
        static PFNGLATTACHSHADERPROC            glAttachShader;
        static PFNGLLINKPROGRAMPROC             glLinkProgram;
        static PFNGLVALIDATEPROGRAMPROC         glValidateProgram;
        static PFNGLGETPROGRAMIVPROC            glGetProgramiv;
        static PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
        static PFNGLGENBUFFERSPROC              glGenBuffers;
        static PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
        static PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
        static PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
        static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
        static PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
        static PFNGLBINDBUFFERPROC              glBindBuffer;
        static PFNGLBINDBUFFERBASEPROC          glBindBufferBase;
        static PFNGLBUFFERDATAPROC              glBufferData;
        static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
        static PFNGLUSEPROGRAMPROC              glUseProgram;
        static PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
        static PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
        static PFNGLDELETEPROGRAMPROC           glDeleteProgram;
        static PFNGLDETACHSHADERPROC            glDetachShader;
        static PFNGLDELETESHADERPROC            glDeleteShader;
        static PFNGLDRAWELEMENTSINSTANCEDPROC   glDrawElementsInstanced;
        static PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
        static PFNGLDEBUGMESSAGECALLBACKPROC    glDebugMessageCallback;
#pragma endregion

        void LoadOpenGLFunctions()
        {
            LOAD_GL_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram);
            LOAD_GL_FUNCTION(PFNGLCREATESHADERPROC, glCreateShader);
            LOAD_GL_FUNCTION(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
            LOAD_GL_FUNCTION(PFNGLUNIFORM1FPROC, glUniform1f);
            LOAD_GL_FUNCTION(PFNGLUNIFORM2FVPROC, glUniform2fv);
            LOAD_GL_FUNCTION(PFNGLUNIFORM3FVPROC, glUniform3fv);
            LOAD_GL_FUNCTION(PFNGLUNIFORM1IPROC, glUniform1i);
            LOAD_GL_FUNCTION(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
            LOAD_GL_FUNCTION(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);
            LOAD_GL_FUNCTION(PFNGLACTIVETEXTUREPROC, glActiveTexture);
            LOAD_GL_FUNCTION(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
            LOAD_GL_FUNCTION(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced);
            LOAD_GL_FUNCTION(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
            LOAD_GL_FUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
            LOAD_GL_FUNCTION(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
            LOAD_GL_FUNCTION(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
            LOAD_GL_FUNCTION(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
            LOAD_GL_FUNCTION(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
            LOAD_GL_FUNCTION(PFNGLBLENDFUNCIPROC, glBlendFunci);
            LOAD_GL_FUNCTION(PFNGLBLENDEQUATIONPROC, glBlendEquation);
            LOAD_GL_FUNCTION(PFNGLCLEARBUFFERFVPROC, glClearBufferfv);
            LOAD_GL_FUNCTION(PFNGLSHADERSOURCEPROC, glShaderSource);
            LOAD_GL_FUNCTION(PFNGLCOMPILESHADERPROC, glCompileShader);
            LOAD_GL_FUNCTION(PFNGLGETSHADERIVPROC, glGetShaderiv);
            LOAD_GL_FUNCTION(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
            LOAD_GL_FUNCTION(PFNGLATTACHSHADERPROC, glAttachShader);
            LOAD_GL_FUNCTION(PFNGLLINKPROGRAMPROC, glLinkProgram);
            LOAD_GL_FUNCTION(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
            LOAD_GL_FUNCTION(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
            LOAD_GL_FUNCTION(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
            LOAD_GL_FUNCTION(PFNGLGENBUFFERSPROC, glGenBuffers);
            LOAD_GL_FUNCTION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
            LOAD_GL_FUNCTION(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
            LOAD_GL_FUNCTION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
            LOAD_GL_FUNCTION(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
            LOAD_GL_FUNCTION(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
            LOAD_GL_FUNCTION(PFNGLBINDBUFFERPROC, glBindBuffer);
            LOAD_GL_FUNCTION(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
            LOAD_GL_FUNCTION(PFNGLBUFFERDATAPROC, glBufferData);
            LOAD_GL_FUNCTION(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv);
            LOAD_GL_FUNCTION(PFNGLUSEPROGRAMPROC, glUseProgram);
            LOAD_GL_FUNCTION(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
            LOAD_GL_FUNCTION(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
            LOAD_GL_FUNCTION(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
            LOAD_GL_FUNCTION(PFNGLDETACHSHADERPROC, glDetachShader);
            LOAD_GL_FUNCTION(PFNGLDELETESHADERPROC, glDeleteShader);
            LOAD_GL_FUNCTION(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced);
            LOAD_GL_FUNCTION(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
            LOAD_GL_FUNCTION(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
        }

        PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB {nullptr};

        GLXContext g_ctx {nullptr};
        Display*   g_display {nullptr};
        ::Window   g_window {0};
        GLuint     g_programID {0};
        GLuint     g_VAO {0};

    } // namespace anonymous

    bool RendererInit(platform::WindowInfoPtr windowInfo)
    {
        if (!windowInfo)
        {
            D_ASSERT(false, "Window info is null.");
            return false;
        }

        i32      screen {XDefaultScreen(*windowInfo->display)};
        ::Window root {XRootWindow(*windowInfo->display, screen)};

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

        i32          fbcount {0};
        GLXFBConfig* fbc {glXChooseFBConfig(*windowInfo->display, screen, visualAttribs, &fbcount)};
        if (!fbc || !fbcount)
        {
            D_ASSERT(false, "Failed to choose X11 framebuffer config");
            return false;
        }
        TRACK_LEAK_ALLOC(fbc, LeakType::OPENGL, "X11 dummy framebuffer config");

        XVisualInfo* vi {glXGetVisualFromFBConfig(*windowInfo->display, fbc[0])};
        if (!vi)
        {
            D_ASSERT(false, "Failed to get X11 visual info");
            return false;
        }
        TRACK_LEAK_ALLOC(vi, LeakType::CUSTOM, "X11 dummy visual info");
        XFree(fbc);
        TRACK_LEAK_FREE(fbc);

        GLXContext dummyCtx {glXCreateContext(*windowInfo->display, vi, nullptr, GL_TRUE)};
        if (!dummyCtx)
        {
            D_ASSERT(false, "Failed to create X11 dummy context");
            return false;
        }
        TRACK_LEAK_ALLOC(dummyCtx, LeakType::OPENGL, "X11 dummy context");
        XFree(vi);
        TRACK_LEAK_FREE(vi);

        if (!glXMakeCurrent(*windowInfo->display, *windowInfo->window, dummyCtx))
        {
            D_ASSERT(false, "Failed to make X11 context current");
            return false;
        }

        LoadOpenGLFunctions(); // Load OpenGL functions.
        LOAD_GL_FUNCTION(PFNGLXCREATECONTEXTATTRIBSARBPROC, glXCreateContextAttribsARB);
        if (!glXCreateContextAttribsARB)
        {
            D_ASSERT(false, "Failed to load glXCreateContextAttribsARB function");
            return false;
        }

        D_TRACE("OpenGL version: %s", glGetString(GL_VERSION));

        glXMakeCurrent(*windowInfo->display, None, nullptr);
        glXDestroyContext(*windowInfo->display, dummyCtx);
        TRACK_LEAK_FREE(dummyCtx);

        return true;
    }

    bool RendererCreateContext(platform::WindowInfoPtr windowInfo, utils::BumpAllocator* transientStorage)
    {
        if (!windowInfo)
        {
            D_ASSERT(false, "Window info is null.");
            return false;
        }

        i32 contextAttribs[] {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
            0};

        g_ctx = glXCreateContextAttribsARB(*windowInfo->display, *windowInfo->fbc, nullptr, GL_TRUE, contextAttribs);
        if (!g_ctx)
        {
            D_ASSERT(false, "Failed to create X11 context");
            return false;
        }
        TRACK_LEAK_ALLOC(g_ctx, LeakType::HANDLE, "X11 context");

        if (!glXMakeCurrent(*windowInfo->display, *windowInfo->window, g_ctx))
        {
            D_ASSERT(false, "Failed to make X11 context current");
            return false;
        }

        // Initalize shaders.
        GLuint vertShaderID {glCreateShader(GL_VERTEX_SHADER)};
        GLuint fragShaderID {glCreateShader(GL_FRAGMENT_SHADER)};

        i32   fileSize {0};
        Char* vertShader {utils::ReadFile("assets/shaders/quad.vert", transientStorage, &fileSize)};
        Char* fragShader {utils::ReadFile("assets/shaders/quad.frag", transientStorage, &fileSize)};
        if (!vertShader || !fragShader)
        {
            D_ASSERT(false, "Failed to read shaders.");
            return false;
        }

        glShaderSource(vertShaderID, 1, &vertShader, 0);
        glShaderSource(fragShaderID, 1, &fragShader, 0);

        glCompileShader(vertShaderID);
        glCompileShader(fragShaderID);

        // Test the vertex shader.
        {
            i32  success {0};
            Char shaderLog[2048] {};

            glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertShaderID, 2048, nullptr, shaderLog);
                D_ASSERT(false, "Failed to compile vertex shader: %s", shaderLog);
                return false;
            }
        }
        // Test the fragment shader.
        {
            i32  success {0};
            Char shaderLog[2048] {};

            glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragShaderID, 2048, nullptr, shaderLog);
                D_ASSERT(false, "Failed to compile fragment shader: %s", shaderLog);
                return false;
            }
        }

        // Create new program.
        g_programID = glCreateProgram();
        glAttachShader(g_programID, vertShaderID);
        glAttachShader(g_programID, fragShaderID);
        glLinkProgram(g_programID);
        TRACK_LEAK_ALLOC(&g_programID, LeakType::OPENGL, "OpenGL program");

        // Delete shaders.
        glDetachShader(g_programID, vertShaderID);
        glDetachShader(g_programID, fragShaderID);
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);

        glGenVertexArrays(1, &g_VAO);
        glBindVertexArray(g_VAO);
        TRACK_LEAK_ALLOC(&g_VAO, LeakType::OPENGL, "OpenGL VAO");

        // Enable depth testing.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);

        glUseProgram(g_programID);

        XFlush(*windowInfo->display);
        g_display = *windowInfo->display;
        g_window  = *windowInfo->window;

        return true;
    }

    void RendererUpdateContext()
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClearDepth(0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, shared::g_screenSize.width, shared::g_screenSize.height);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glXSwapBuffers(g_display, g_window);
    }

    void RendererDestroyContext()
    {
        glXMakeCurrent(g_display, None, nullptr);
        glXDestroyContext(g_display, g_ctx);
        TRACK_LEAK_FREE(g_ctx);
        g_ctx     = nullptr;
        g_display = nullptr;
        g_window  = 0;
    }

    void RendererShutdown()
    {
        glDeleteVertexArrays(1, &g_VAO);
        TRACK_LEAK_FREE(&g_VAO);
        glDeleteProgram(g_programID);
        TRACK_LEAK_FREE(&g_programID);
    }

} // namespace drop::renderer