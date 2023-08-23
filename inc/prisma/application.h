#ifndef PRISMA_APPLICATION_H
#define PRISMA_APPLICATION_H

#include <stdint.h>

#include "prisma/error.h"
#include "prisma/renderer.h"
#include "prisma/ui.h"
#include "prisma/window.h"

struct prisma_application_info
{
    uint32_t default_width;
    uint32_t default_height;
    bool fullscreen;
    const char *application_name;
};

struct prisma_application
{
    struct prisma_renderer renderer;
    struct prisma_ui ui;
    struct prisma_window window;
};

/*
 * @brief Init Prisma application
 * @param application Prisma application
 * @param info intial information about Prisma application
 * @return Error if any
 */
enum prisma_error prisma_application_init(struct prisma_application *application,
                                          struct prisma_application_info *info);

/*
 * @brief run Prisma application
 * @param application Prisma application
 * @return Error if any
 */
enum prisma_error prisma_application_run(struct prisma_application *application);

/*
 * @brief destroy Prisma application
 * @param application Prisma application
 */
void prisma_application_destroy(struct prisma_application *application);

#endif /* PRISMA_APPLICATION_H */