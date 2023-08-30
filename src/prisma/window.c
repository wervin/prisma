#include "prisma/application.h"
#include "prisma/log.h"
#include "prisma/window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

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
        PRISMA_LOG_ERROR(PRISMA_ERROR_GLFW, "Failed to create window");
        return PRISMA_ERROR_GLFW;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_window_show()
{
    glfwShowWindow(_window.glfw_window);
}

void prisma_window_get_extent(int *width, int *height)
{
    glfwGetFramebufferSize(_window.glfw_window, width, height);
}

bool prisma_window_should_close()
{
    return glfwWindowShouldClose(_window.glfw_window);
}

void prisma_window_wait_events()
{
    glfwWaitEvents();
}

enum prisma_error prisma_window_create_surface(void * instance, void *surface)
{
    if (glfwCreateWindowSurface(*(VkInstance*) instance, _window.glfw_window, NULL, (VkSurfaceKHR*) surface) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_GLFW, "Failed to create window surface");
        return PRISMA_ERROR_GLFW;
    }
    return PRISMA_ERROR_NONE;
}

const char** prisma_window_get_required_extensions(uint32_t* count)
{
    return glfwGetRequiredInstanceExtensions(count);
}

void prisma_window_poll_events()
{
    glfwPollEvents();
}

void prisma_window_destroy()
{
    glfwDestroyWindow(_window.glfw_window);
    glfwTerminate();
}