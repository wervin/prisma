#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>
#include <sake/vector.h>

#include "prisma/components/editor.h"

#include "prisma/log.h"
#include "prisma/style.h"

struct prisma_editor * prisma_editor_new(void)
{
    struct prisma_editor *editor;

    editor = calloc(1, sizeof(struct prisma_editor));
    if (!editor)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate editor");
        return NULL;
    }

    struct poulpe_style_info style_info = {
        .large_font = prisma_style.large_font,
        .small_font = prisma_style.small_font,
        .theme = POULPE_THEME_DARK
    };

    poulpe_style_set(&style_info);

    editor->editors = sake_vector_new(sizeof(struct poulpe_editor *), (void (*) (void*)) poulpe_editor_free);
    if (!editor->editors)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate editors");
        return NULL;
    }

    return editor;
}

void prisma_editor_free(struct prisma_editor *editor)
{
    if (editor->editors)
        sake_vector_free(editor->editors);
    free(editor);
}

enum prisma_error prisma_editor_draw(struct prisma_editor *editor)
{

    for (uint32_t i = 0; i < sake_vector_size(editor->editors); i++)
    {
        const char *filename = poulpe_editor_filename(editor->editors[i]);
        if (igBegin(filename, NULL, 0))
            poulpe_editor_draw(editor->editors[i]);
        igEnd();
    }

    return PRISMA_ERROR_NONE;
}

void prisma_editor_set_ui(struct prisma_editor *editor, struct prisma_ui *ui)
{
    editor->ui = ui;
}

enum prisma_error prisma_editor_open(struct prisma_editor *editor, const char *path)
{
    struct poulpe_editor *poulpe_editor = poulpe_editor_new(path);
    if (!poulpe_editor)
        return PRISMA_ERROR_MEMORY;

    editor->editors = sake_vector_push_back(editor->editors, &poulpe_editor);
    if (!editor->editors)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot open new editor");
        return PRISMA_ERROR_MEMORY;
    }

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_editor_close(struct prisma_editor *editor, const char *filename)
{

}
