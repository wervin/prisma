#include "prisma/application.h"
#include "prisma/log.h"
#include "prisma/window.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cimgui_impl.h>

#include "sake/macro.h"

struct _window
{
    GLFWwindow *glfw_window;
};

static struct _window _window = {0};

enum prisma_error prisma_window_init(struct prisma_window_info *info)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (info->fullscreen)
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    _window.glfw_window = glfwCreateWindow(
        info->default_width,
        info->default_height,
        info->application_name, NULL, NULL);
    if (!_window.glfw_window)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_GLFW, "Failed to create window");
        return PRISMA_ERROR_GLFW;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_window_show(void)
{
    glfwShowWindow(_window.glfw_window);
}

void prisma_window_get_extent(int *width, int *height)
{
    glfwGetFramebufferSize(_window.glfw_window, width, height);
}

bool prisma_window_should_close(void)
{
    return glfwWindowShouldClose(_window.glfw_window);
}

void prisma_window_wait_events(void)
{
    glfwWaitEvents();
}

enum prisma_error prisma_window_create_surface(void * instance, void *surface)
{
    if (glfwCreateWindowSurface(*(VkInstance*) instance, _window.glfw_window, NULL, (VkSurfaceKHR*) surface) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_GLFW, "Failed to create window surface");
        return PRISMA_ERROR_GLFW;
    }
    return PRISMA_ERROR_NONE;
}

const char** prisma_window_get_required_extensions(uint32_t* count)
{
    return glfwGetRequiredInstanceExtensions(count);
}

void prisma_window_poll_events(void)
{
    glfwPollEvents();
}

void prisma_window_destroy(void)
{
    glfwDestroyWindow(_window.glfw_window);
    glfwTerminate();
}

enum prisma_error primsa_window_init_ui(void)
{
    if (!ImGui_ImplGlfw_InitForVulkan(_window.glfw_window, true))
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_GLFW, "Failed to instantiate GLFw for ImGUI");
        return PRISMA_ERROR_GLFW; 
    }
    return PRISMA_ERROR_NONE;
}

void prisma_window_draw_ui(void)
{
    ImGui_ImplGlfw_NewFrame();
}

void prisma_window_destroy_ui(void)
{
    ImGui_ImplGlfw_Shutdown();
}