#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "prisma/components/viewport.h"

#include "prisma/log.h"
#include "prisma/renderer.h"

struct prisma_viewport * prisma_viewport_new(void)
{
    struct prisma_viewport *viewport;

    viewport = calloc(1, sizeof(struct prisma_viewport));
    if (!viewport)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate viewport");
        return NULL;
    }

    enum prisma_error error = prisma_renderer_init_viewport();
    if (error != PRISMA_ERROR_NONE)
        return NULL;

    return viewport;
}

void prisma_viewport_free(struct prisma_viewport *viewport)
{
    prisma_renderer_destroy_viewport();
    free(viewport);
}

enum prisma_error prisma_viewport_draw(struct prisma_viewport *viewport)
{
    SAKE_MACRO_UNUSED(viewport);

    prisma_renderer_draw_viewport();
    return PRISMA_ERROR_NONE;
}

void prisma_viewport_set_ui(struct prisma_viewport *viewport, struct prisma_ui *ui)
{
    viewport->ui = ui;
}