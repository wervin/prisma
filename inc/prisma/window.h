#ifndef PRISMA_WINDOW_H
#define PRISMA_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "prisma/error.h"

struct prisma_application;

struct prisma_window_info
{
    uint32_t default_width;
    uint32_t default_height;
    bool fullscreen;
    const char *application_name;
};

struct prisma_window
{
    GLFWwindow *glfw_window;
};

/*
 * @brief Init Prisma window
 * @param window Prisma window
 * @param application Prisma application
 * @param info intial information about Prisma window
 * @return Error if any
 */
enum prisma_error prisma_window_init(struct prisma_window *window,
                                     struct prisma_application *application,
                                     struct prisma_window_info *info);

/*
 * @brief Makes the Prisma window visible
 * @param window Prisma window
 */
void prisma_window_show(struct prisma_window *window);

/*
 * @brief Checks the close flag from Prisma window
 * @param window Prisma window
 */
bool prisma_window_should_close(struct prisma_window *window);

/*
 * @brief Processes all pending events from Prisma window
 * @param window Prisma window
 */
void prisma_window_poll_events(struct prisma_window *window);

/*
 * @brief destroy Prisma window
 * @param window Prisma window
 */
void prisma_window_destroy(struct prisma_window *window);

#endif /* PRISMA_WINDOW_H */