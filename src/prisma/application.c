#include "prisma/application.h"
#include "prisma/renderer.h"
#include "prisma/ui.h"
#include "prisma/window.h"

enum prisma_error prisma_application_init(struct prisma_application_info *info)
{   
    struct prisma_window_info window_info = {
        .default_width = info->default_width,
        .default_height = info->default_height,
        .fullscreen = info->fullscreen,
        .application_name = info->application_name
    };
    
    prisma_window_init(&window_info);
    prisma_renderer_init();
    prisma_window_show();

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_application_run()
{    
    while (!prisma_window_should_close())
    {
        prisma_window_poll_events();
        prisma_ui_draw();
        prisma_renderer_draw();
    }

    return PRISMA_ERROR_NONE;
}

void prisma_application_destroy()
{
    prisma_ui_destroy();
    prisma_renderer_destroy();
    prisma_window_destroy();
}