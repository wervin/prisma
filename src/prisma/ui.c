#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include "prisma/ui.h"
#include "prisma/renderer.h"
#include "prisma/window.h"

struct _ui
{
    ImGuiContext *imgui_context;
    ImGuiIO *imgui_io;
};

static struct _ui _ui = {0};

enum prisma_error prisma_ui_init(void)
{
    enum prisma_error status = PRISMA_ERROR_NONE;

    _ui.imgui_context = igCreateContext(NULL);
    _ui.imgui_io = igGetIO();
    _ui.imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // _ui.imgui_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    status = primsa_window_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_viewport();
    if (status != PRISMA_ERROR_NONE)
        return status;

    return PRISMA_ERROR_NONE;
}

void prisma_ui_draw(void)
{   
    prisma_window_refresh_ui();
    prisma_renderer_refresh_ui();

    igNewFrame();

    igShowDemoWindow(NULL);

    prisma_renderer_draw_ui();

    prisma_renderer_draw_viewport();
    
    igRender();
    // igUpdatePlatformWindows();
    // igRenderPlatformWindowsDefault(NULL, NULL);
}

void prisma_ui_destroy(void)
{
    prisma_renderer_destroy_ui();
    prisma_window_destroy_ui();
    igDestroyContext(_ui.imgui_context);
}