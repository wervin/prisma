#include "prisma/application.h"

enum prisma_error prisma_application_init(struct prisma_application *application,
                                          struct prisma_application_info *info)
{   
    struct prisma_window_info window_info = {
        .default_width = info->default_width,
        .default_height = info->default_height,
        .fullscreen = info->fullscreen,
        .application_name = info->application_name
    };
    
    prisma_window_init(&application->window, application, &window_info);

    struct prisma_renderer_info renderer_info = {0};
    prisma_renderer_init(&application->renderer, &renderer_info);

    struct prisma_ui_info ui_info = {0};
    prisma_ui_init(&application->ui, &ui_info);

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_application_run(struct prisma_application *application)
{
    prisma_window_show(&application->window);
    
    while (!prisma_window_should_close(&application->window))
        prisma_window_poll_events(&application->window);

    return PRISMA_ERROR_NONE;
}

void prisma_application_destroy(struct prisma_application *application)
{
    prisma_ui_destroy(&application->ui);
    prisma_renderer_destroy(&application->renderer);
    prisma_window_destroy(&application->window);
}