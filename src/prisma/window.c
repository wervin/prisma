#include "prisma/application.h"
#include "prisma/log.h"
#include "prisma/window.h"

#include "sake/macro.h"

enum prisma_error prisma_window_init(struct prisma_window *window,
                                     struct prisma_window_info *info)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (info->fullscreen)
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window->glfw_window = glfwCreateWindow(
        info->default_width,
        info->default_height,
        info->application_name, NULL, NULL);
    if (!window->glfw_window)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_GLFW, "Failed to create window");
        return PRISMA_ERROR_GLFW;
    }

    // glfwSetWindowUserPointer(application->window.glfw_window, application);

    return PRISMA_ERROR_NONE;
}

void prisma_window_show(struct prisma_window *window)
{
    glfwShowWindow(window->glfw_window);
}

VkExtent2D prisma_window_get_extent(struct prisma_window *window)
{
    int width, height;
    glfwGetFramebufferSize(window->glfw_window, &width, &height);
    return (VkExtent2D){.height = height, .width = width};
}

bool prisma_window_should_close(struct prisma_window *window)
{
    return glfwWindowShouldClose(window->glfw_window);
}

void prisma_window_wait_events(struct prisma_window *window)
{
    SAKE_MACRO_UNUSED(window);
    glfwWaitEvents();
}

void prisma_window_poll_events(struct prisma_window *window)
{
    SAKE_MACRO_UNUSED(window);
    glfwPollEvents();
}

void prisma_window_destroy(struct prisma_window *window)
{
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
}