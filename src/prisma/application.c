#include <pthread.h>

#include "prisma/application.h"
#include "prisma/backend.h"
#include "prisma/renderer.h"
#include "prisma/ui.h"
#include "prisma/window.h"

enum prisma_error prisma_application_init(struct prisma_application_info *info)
{   
    enum prisma_error status = PRISMA_ERROR_NONE;

    struct prisma_window_info window_info = {
        .default_width = info->default_width,
        .default_height = info->default_height,
        .fullscreen = info->fullscreen,
        .application_name = info->application_name
    };
    
    status = prisma_window_init(&window_info);
    if (status != PRISMA_ERROR_NONE)
        return status;
    
    status = prisma_renderer_init();
    if (status != PRISMA_ERROR_NONE)
        return status;
    
    status = prisma_ui_init();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_init();
    if (status != PRISMA_ERROR_NONE)
        return status;
    
    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_application_run()
{    
    prisma_window_show();
    
    while (!prisma_window_should_close())
    {
        prisma_window_poll_events();
        prisma_ui_draw();
        prisma_renderer_draw();
    }
    
    prisma_renderer_wait_idle();

    return PRISMA_ERROR_NONE;
}

void prisma_application_destroy()
{
    prisma_backend_destroy();
    prisma_ui_destroy();
    prisma_renderer_destroy();
    prisma_window_destroy();
}