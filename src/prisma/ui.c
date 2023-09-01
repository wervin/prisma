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
    
    status = primsa_window_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_ui_draw(void)
{
    enum prisma_error status = PRISMA_ERROR_NONE;
   
    status = prisma_window_refresh_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_refresh_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    return PRISMA_ERROR_NONE;
}

void prisma_ui_destroy(void)
{
    prisma_renderer_destroy_ui();
    prisma_window_destroy_ui();
    igDestroyContext(_ui.imgui_context);
}