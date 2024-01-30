#include <stdlib.h>
#include <string.h>

#include <cimgui.h>

#include <poulpe.h>

#include <sake/macro.h>
#include <sake/vector.h>

#include "prisma/components/editor.h"

#include "prisma/ui.h"
#include "prisma/log.h"
#include "prisma/style.h"
#include "prisma/renderer.h"

enum prisma_error _update(struct prisma_editor *editor);
enum prisma_error _close_context(struct prisma_editor *editor, uint32_t index);
void _free_context(struct prisma_editor_context *context);

struct prisma_editor * prisma_editor_new(void)
{
    struct prisma_editor *editor;

    editor = calloc(1, sizeof(struct prisma_editor));
    if (!editor)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate editor");
        return NULL;
    }

    struct poulpe_style_info style_info = {
        .large_font = prisma_style.large_font,
        .small_font = prisma_style.small_font,
        .theme = POULPE_THEME_DARK
    };

    poulpe_style_set(&style_info);

    editor->contexts = sake_vector_new(sizeof(struct prisma_editor_context *), (void (*)(void *))_free_context);
    if (!editor->contexts)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate contexts");
        return NULL;
    }

    editor->current = NULL;

    if (_update(editor) != PRISMA_ERROR_NONE)
        return NULL;

    return editor;
}

void prisma_editor_free(struct prisma_editor *editor)
{
    if (editor->contexts)
        sake_vector_free(editor->contexts);
    free(editor);
}

enum prisma_error prisma_editor_draw(struct prisma_editor *editor)
{
    enum prisma_error error = PRISMA_ERROR_NONE;

    if (sake_vector_size(editor->contexts))
    {
        for (uint32_t i = 0; i < sake_vector_size(editor->contexts); i++)
        {
            struct prisma_editor_context *context = editor->contexts[i];
            ImGuiWindowFlags flags = context->temporary ? ImGuiWindowFlags_UnsavedDocument : 0;
            if (igBegin(prisma_editor_context_filename(context), &context->open, flags))
            {
                if (poulpe_editor_draw(context->poulpe) != POULPE_ERROR_NONE)
                {   
                    igEnd();
                    return PRISMA_ERROR_POULPE;
                }

                if (editor->current != context)
                {
                    error = prisma_renderer_update_viewport(poulpe_editor_path(context->poulpe));
                    if (error != PRISMA_ERROR_NONE)
                    {
                        igEnd();
                        return error;
                    }
                    editor->current = context;
                }
            }

            if (!context->open)
            {
                error = _close_context(editor, i);
                if (error != PRISMA_ERROR_NONE)
                {
                    igEnd();
                    return error;
                }
            }

            igEnd();
        }
    }

    return error;
}

void prisma_editor_set_ui(struct prisma_editor *editor, struct prisma_ui *ui)
{
    editor->ui = ui;
}

const char *prisma_editor_context_filename(struct prisma_editor_context *context)
{
    return context->temporary ? "Untitled.frag" : poulpe_editor_filename(context->poulpe);
}

enum prisma_error prisma_editor_open(struct prisma_editor *editor, const char *path)
{
    for (uint32_t i = 0; i < sake_vector_size(editor->contexts); i++)
    {
        struct prisma_editor_context *context = editor->contexts[i];
        if (strcmp(poulpe_editor_path(context->poulpe), path) == 0)
            return PRISMA_ERROR_NONE;
    }

    struct poulpe_editor *poulpe_editor = poulpe_editor_new(path);
    if (!poulpe_editor)
        return PRISMA_ERROR_MEMORY;

    struct prisma_editor_context *context = calloc(1, sizeof(struct prisma_editor_context));
    if (!context)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate new context");
        return PRISMA_ERROR_MEMORY;
    }

    context->poulpe = poulpe_editor;
    context->temporary = false;
    context->open = true;

    editor->contexts = sake_vector_push_back(editor->contexts, &context);
    if (!editor->contexts)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot open new editor");
        return PRISMA_ERROR_MEMORY;
    }

    return PRISMA_ERROR_NONE;
}

enum prisma_error _update(struct prisma_editor *editor)
{
    if (sake_vector_size(editor->contexts))
        return PRISMA_ERROR_NONE;

    int32_t c;

    const char *default_path = "assets/shaders/default.frag";
    FILE *default_fp;

    char tmp_path[] = "/tmp/prismaXXXXXX";
    int tmp_fd;
    FILE *tmp_fp;

    default_fp = fopen(default_path, "r");
    if (!default_fp)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot open default frag shader file");
        return PRISMA_ERROR_MEMORY;
    }

    tmp_fd = mkstemp(tmp_path);
    if (tmp_fd == -1)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot create temporary file");
        fclose(default_fp);
        return PRISMA_ERROR_MEMORY;
    }

    tmp_fp = fdopen(tmp_fd, "w");
    if (!tmp_fp)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot create temporary file");
        fclose(default_fp);
        remove(tmp_path);
        return PRISMA_ERROR_MEMORY;
    }

    while( (c = fgetc(default_fp)) != EOF )
        fputc(c, tmp_fp);

    fclose(default_fp);
    fclose(tmp_fp);
  
    struct poulpe_editor *poulpe_editor = poulpe_editor_new(tmp_path);
    if (!poulpe_editor)
        return PRISMA_ERROR_MEMORY;

    if (poulpe_editor_set_language(poulpe_editor, POULPE_LANGUAGE_TYPE_GLSL) != POULPE_ERROR_NONE)
        return PRISMA_ERROR_POULPE;

    struct prisma_editor_context *context = calloc(1, sizeof(struct prisma_editor_context));
    if (!context)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate new context");
        return PRISMA_ERROR_MEMORY;
    }

    context->poulpe = poulpe_editor;
    context->temporary = true;
    context->open = true;

    editor->contexts = sake_vector_push_back(editor->contexts, &context);
    if (!editor->contexts)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot push back new context");
        return PRISMA_ERROR_MEMORY;
    }

    return PRISMA_ERROR_NONE;
}

enum prisma_error _close_context(struct prisma_editor *editor, uint32_t index)
{
    sake_vector_erase(editor->contexts, index);
    return _update(editor);
}

void _free_context(struct prisma_editor_context *context)
{
    if (context->temporary)
        remove(poulpe_editor_path(context->poulpe));
    poulpe_editor_free(context->poulpe);
    free(context);
}