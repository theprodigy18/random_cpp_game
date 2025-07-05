#include "renderer/opengl.hpp"
#include "utils/file_io.hpp"
#include "shared/input.hpp"

#include "opengl/wglext.h"
#include <gl/GL.h>

namespace drop::renderer
{
    namespace
    {
        HMODULE g_openglDLL {nullptr};

#define LOAD_GL_FUNCTION(type, name)                      \
    name = (type) wglGetProcAddress(#name);               \
    if (!name)                                            \
    {                                                     \
        name = (type) GetProcAddress(g_openglDLL, #name); \
    }                                                     \
    D_ASSERT(name, "Failed to load OpenGL function: %s", #name);

#pragma region OpenGL Functions
        static PFNGLCREATEPROGRAMPROC           glCreateProgram;
        static PFNGLDELETETEXTURESPROC          glDeleteTextures;
        static PFNGLGENTEXTURESPROC             glGenTextures;
        static PFNGLBINDTEXTUREPROC             glBindTexture;
        static PFNGLDRAWBUFFERPROC              glDrawBuffer;
        static PFNGLDRAWARRAYSPROC              glDrawArrays;
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
            g_openglDLL = LoadLibraryA("opengl32.dll");
            D_ASSERT(g_openglDLL, "Failed to load opengl32.dll");
            LOAD_GL_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram);
            LOAD_GL_FUNCTION(PFNGLDELETETEXTURESPROC, glDeleteTextures);
            LOAD_GL_FUNCTION(PFNGLGENTEXTURESPROC, glGenTextures);
            LOAD_GL_FUNCTION(PFNGLBINDTEXTUREPROC, glBindTexture);
            LOAD_GL_FUNCTION(PFNGLDRAWBUFFERPROC, glDrawBuffer);
            LOAD_GL_FUNCTION(PFNGLDRAWARRAYSPROC, glDrawArrays);
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

        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB {nullptr};
        PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB {nullptr};

        HGLRC  g_hglrc {nullptr};
        HDC    g_hdc {nullptr};
        GLuint g_programID {0};
        GLuint g_VAO {0};

        void CALLBACK GLDebugCallback(GLenum source, GLenum type, GLuint id,
                                      GLenum severity, GLsizei length, const GLchar* message,
                                      const void* userParam)
        {
            if (severity == GL_DEBUG_SEVERITY_LOW ||
                severity == GL_DEBUG_SEVERITY_MEDIUM ||
                severity == GL_DEBUG_SEVERITY_HIGH)
            {
                D_ASSERT(false, "OpenGL error: %s", message);
            }
            else
            {
                D_TRACE((Char*) message);
            }
        }

    } // namespace anonymous

    bool RendererInit(platform::WindowInfoPtr windowInfo)
    {
        if (!windowInfo)
        {
            D_ASSERT(false, "Window info is null.");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd {};
        pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType   = PFD_TYPE_RGBA;
        pfd.cColorBits   = 32;
        pfd.cDepthBits   = 24;
        pfd.cStencilBits = 8;

        i32 pixelFormat {ChoosePixelFormat(*windowInfo->hdc, &pfd)};
        if (pixelFormat == 0)
        {
            D_ASSERT(false, "ChoosePixelFormat failed");
            return false;
        }

        if (!SetPixelFormat(*windowInfo->hdc, pixelFormat, &pfd))
        {
            D_ASSERT(false, "SetPixelFormat failed");
            return false;
        }

        HGLRC rc {wglCreateContext(*windowInfo->hdc)};
        if (!rc)
        {
            D_ASSERT(false, "Failed to create OpenGL context.");
            return false;
        }
        TRACK_LEAK_ALLOC(rc, LeakType::OPENGL, "OpenGL context");

        if (!wglMakeCurrent(*windowInfo->hdc, rc))
        {
            D_ASSERT(false, "Failed to make OpenGL context current.");
            return false;
        }

        LoadOpenGLFunctions(); // Load OpenGL functions.
        LOAD_GL_FUNCTION(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
        LOAD_GL_FUNCTION(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
        if (!wglCreateContextAttribsARB || !wglChoosePixelFormatARB)
        {
            D_ASSERT(false, "Failed to load OpenGL functions.");
            return false;
        }

        D_TRACE("OpenGL version: %s", glGetString(GL_VERSION));

        // Enable debug messages.
        glDebugMessageCallback(&GLDebugCallback, nullptr);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_DEBUG_OUTPUT);

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(rc);
        TRACK_LEAK_FREE(rc);

        return true;
    }

    bool RendererCreateContext(platform::WindowInfoPtr windowInfo, utils::BumpAllocator* transientStorage)
    {
        if (!windowInfo)
        {
            D_ASSERT(false, "Window info is null.");
            return false;
        }

        const i32 pixelAttribs[] {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_ALPHA_BITS_ARB, 8,
            0}; // Last entry must be 0 to terminate the list.

        u32 numPixelFormats {0};
        i32 pixelFormat {0};
        if (!wglChoosePixelFormatARB(*windowInfo->hdc, pixelAttribs, nullptr, 1, &pixelFormat, &numPixelFormats))
        {
            D_ASSERT(false, "Failed to choose pixel format.");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd {};
        if (!DescribePixelFormat(*windowInfo->hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
        {
            D_ASSERT(false, "Failed to describe pixel format.");
            return false;
        }

        if (!SetPixelFormat(*windowInfo->hdc, pixelFormat, &pfd))
        {
            D_ASSERT(false, "Failed to set pixel format.");
            return false;
        }

        const i32 contextAttribs[] {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0}; // Last entry must be 0 to terminate the list.

        g_hglrc = wglCreateContextAttribsARB(*windowInfo->hdc, nullptr, contextAttribs);
        if (!g_hglrc)
        {
            D_ASSERT(false, "Failed to create OpenGL context.");
            return false;
        }
        TRACK_LEAK_ALLOC(g_hglrc, LeakType::OPENGL, "OpenGL context");

        if (!wglMakeCurrent(*windowInfo->hdc, g_hglrc))
        {
            D_ASSERT(false, "Failed to make OpenGL context current.");
            return false;
        }

        g_hdc = *windowInfo->hdc;

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

        UpdateWindow(*windowInfo->hwnd);

        return true;
    }

    void RendererUpdateContext()
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClearDepth(0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, shared::g_screenSize.width, shared::g_screenSize.height);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        SwapBuffers(g_hdc);
    }

    void RendererDestroyContext()
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(g_hglrc);
        TRACK_LEAK_FREE(g_hglrc);
        g_hglrc = nullptr;
        g_hdc   = nullptr;
    }

    void RendererShutdown()
    {
        FreeLibrary(g_openglDLL);
        g_openglDLL = nullptr;

        glDeleteVertexArrays(1, &g_VAO);
        TRACK_LEAK_FREE(&g_VAO);
        glDeleteProgram(g_programID);
        TRACK_LEAK_FREE(&g_programID);
    }

} // namespace drop::renderer