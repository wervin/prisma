#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "prisma/components/menu.h"

#include "prisma/log.h"

struct prisma_menu * prisma_menu_new(void)
{
    struct prisma_menu *menu;

    menu = calloc(1, sizeof(struct prisma_menu));
    if (!menu)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate menu");
        return NULL;
    }

    return menu;
}

void prisma_menu_free(struct prisma_menu *menu)
{
    free(menu);
}

enum prisma_error prisma_menu_draw(struct prisma_menu *menu)
{
    SAKE_MACRO_UNUSED(menu);

    if (igBeginMainMenuBar())
    {
        if (igBeginMenu("File", true))
        {
            if (igMenuItem_Bool("New", NULL, false, true))
            {
            }
            if (igMenuItem_Bool("Open", NULL, false, true))
            {
            }
            if (igMenuItem_Bool("Save", NULL, false, true))
            {
            }
            if (igMenuItem_Bool("Save As...", NULL, false, true))
            {
            }
            igEndMenu();
        }

        igEndMainMenuBar();
    }

    return PRISMA_ERROR_NONE;
}