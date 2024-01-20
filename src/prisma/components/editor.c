#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "prisma/components/editor.h"

#include "prisma/log.h"

struct prisma_editor * prisma_editor_new(void)
{
    struct prisma_editor *editor;

    editor = calloc(1, sizeof(struct prisma_editor));
    if (!editor)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate editor");
        return NULL;
    }

    return editor;
}

void prisma_editor_free(struct prisma_editor *editor)
{
    free(editor);
}

enum prisma_error prisma_editor_draw(struct prisma_editor *editor)
{
    SAKE_MACRO_UNUSED(editor);

    return PRISMA_ERROR_NONE;
}