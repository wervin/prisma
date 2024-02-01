#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/inotify.h>

#include <cimgui.h>

#include <poulpe.h>

#include <sake/macro.h>
#include <sake/vector.h>

#include "prisma/components/editor.h"

#include "prisma/ui.h"
#include "prisma/log.h"
#include "prisma/style.h"
#include "prisma/renderer.h"

static enum prisma_error _close_context(struct prisma_editor *editor, uint32_t index);
static void _free_context(struct prisma_editor_context *context);
static void* _inotify(void *args);

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

    editor->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

    editor->current = NULL;

    if (prisma_editor_new_file(editor) != PRISMA_ERROR_NONE)
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

    pthread_mutex_lock(&editor->mutex);

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
                    error = PRISMA_ERROR_POULPE;
                    goto end;
                }

                if (editor->current != context)
                {
                    prisma_renderer_request_viewport_update(poulpe_editor_path(context->poulpe));
                    editor->current = context;
                }
            }
            igEnd();
        }
    }

    uint32_t i = 0;
    while (i <  sake_vector_size(editor->contexts))
    {
        struct prisma_editor_context *context = editor->contexts[i];
        if (!context->open)
        {
            error = _close_context(editor, i);
            if (error != PRISMA_ERROR_NONE)
                goto end;
        }
        else
            i++;
    }

end:
    pthread_mutex_unlock(&editor->mutex);
    if (sake_vector_size(editor->contexts))
        return error;
    return prisma_editor_new_file(editor);
}

void prisma_editor_set_ui(struct prisma_editor *editor, struct prisma_ui *ui)
{
    editor->ui = ui;
}

const char *prisma_editor_context_filename(struct prisma_editor_context *context)
{
    return poulpe_editor_filename(context->poulpe);
}

enum prisma_error prisma_editor_open_file(struct prisma_editor *editor, const char *path)
{
    enum prisma_error error = PRISMA_ERROR_NONE;
    pthread_mutex_lock(&editor->mutex);

    for (uint32_t i = 0; i < sake_vector_size(editor->contexts); i++)
    {
        struct prisma_editor_context *context = editor->contexts[i];
        if (strcmp(poulpe_editor_path(context->poulpe), path) == 0)
        {
            error = PRISMA_ERROR_NONE;
            goto end;
        }
    }

    PRISMA_LOG_DEBUG("Open %s\n", path);

    struct poulpe_editor *poulpe_editor = poulpe_editor_new(path);
    if (!poulpe_editor)
    {
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    struct prisma_editor_context *context = calloc(1, sizeof(struct prisma_editor_context));
    if (!context)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate new context");
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    context->poulpe = poulpe_editor;
    context->temporary = false;
    context->open = true;
    pthread_create(&context->thread_id , NULL, _inotify, context);

    editor->contexts = sake_vector_push_back(editor->contexts, &context);
    if (!editor->contexts)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot open new editor");
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

end:
    pthread_mutex_unlock(&editor->mutex);
    return error;
}

enum prisma_error prisma_editor_new_file(struct prisma_editor *editor)
{
    int32_t c;

    const char *default_path = "assets/shaders/default.frag";
    FILE *default_fp;

    char tmp_path[] = "/tmp/prismaXXXXXX.frag";
    int tmp_fd;
    FILE *tmp_fp;

    enum prisma_error error = PRISMA_ERROR_NONE;

    pthread_mutex_lock(&editor->mutex);

    default_fp = fopen(default_path, "r");
    if (!default_fp)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot open default frag shader file");
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    tmp_fd = mkstemps(tmp_path, sizeof(".frag")- 1);
    if (tmp_fd == -1)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot create temporary file");
        fclose(default_fp);
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    tmp_fp = fdopen(tmp_fd, "w");
    if (!tmp_fp)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot create temporary file");
        fclose(default_fp);
        remove(tmp_path);
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    while( (c = fgetc(default_fp)) != EOF )
        fputc(c, tmp_fp);

    fclose(default_fp);
    fclose(tmp_fp);
  
    struct poulpe_editor *poulpe_editor = poulpe_editor_new(tmp_path);
    if (!poulpe_editor)
    {
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    struct prisma_editor_context *context = calloc(1, sizeof(struct prisma_editor_context));
    if (!context)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate new context");
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    context->poulpe = poulpe_editor;                                                                                                                                            
    context->temporary = true;
    context->open = true;
    pthread_create(&context->thread_id , NULL, _inotify, context);

    editor->contexts = sake_vector_push_back(editor->contexts, &context);
    if (!editor->contexts)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot push back new context");
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

end:
    pthread_mutex_unlock(&editor->mutex);
    return error;
}

enum prisma_error prisma_editor_copy_file(struct prisma_editor *editor, const char *path)
{
    int32_t c;
    FILE *src_fp;
    FILE *dest_fp;

    enum prisma_error error = PRISMA_ERROR_NONE;

    pthread_mutex_lock(&editor->mutex);

    src_fp = fopen(poulpe_editor_path(editor->current->poulpe), "r");
    if (!src_fp)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot open source file");
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    dest_fp = fopen(path, "w");
    if (!dest_fp)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot open destination file");
        fclose(src_fp);
        error = PRISMA_ERROR_MEMORY;
        goto end;
    }

    while( (c = fgetc(src_fp)) != EOF )
        fputc(c, dest_fp);

    fclose(src_fp);
    fclose(dest_fp);

end:
    pthread_mutex_unlock(&editor->mutex);
    return error;
}

enum prisma_error prisma_editor_close_file(struct prisma_editor *editor)
{
    if (!editor->current)
        return PRISMA_ERROR_FILE;
    
    editor->current->open = false;
    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_editor_save_as_file(struct prisma_editor *editor, const char *path)
{
    enum prisma_error error = PRISMA_ERROR_NONE;
    
    pthread_mutex_lock(&editor->mutex);

    uint32_t i = 0;
    while (i < sake_vector_size(editor->contexts))
    {
        if (strcmp(path, poulpe_editor_path(editor->contexts[i]->poulpe)) == 0)
            break;
        i++;
    }

    if (i < sake_vector_size(editor->contexts))
    {
        struct prisma_editor_context *context = editor->contexts[i];
        context->open = false;
        context->temporary = true;
        
        error = _close_context(editor, i);
        if (error != PRISMA_ERROR_NONE)
        {
            pthread_mutex_unlock(&editor->mutex);
            goto end;
        }
    }

    pthread_mutex_unlock(&editor->mutex);

    error = prisma_editor_copy_file(editor, path);
    if (error != PRISMA_ERROR_NONE)
        goto end;

    error = prisma_editor_open_file(editor, path);
    if (error != PRISMA_ERROR_NONE)
        goto end;

    error = prisma_editor_close_file(editor);
    if (error != PRISMA_ERROR_NONE)
        goto end;
    
end:
    return error;
}

static enum prisma_error _close_context(struct prisma_editor *editor, uint32_t index)
{
    PRISMA_LOG_DEBUG("Close %s\n", poulpe_editor_path(editor->contexts[index]->poulpe));

    if (pthread_join(editor->contexts[index]->thread_id, NULL) != 0)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot join context thread");
        return PRISMA_ERROR_MEMORY;
    }

    sake_vector_erase(editor->contexts, index);
    return PRISMA_ERROR_NONE;
}

static void _free_context(struct prisma_editor_context *context)
{
    if (context->temporary)
        remove(poulpe_editor_path(context->poulpe));

    poulpe_editor_free(context->poulpe);
    free(context);
}

static void* _inotify(void *args)
{
    struct prisma_editor_context *context = (struct prisma_editor_context *) args;
    
    int32_t fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot init inotify");
        pthread_exit(NULL);
    }

    if (inotify_add_watch(fd, poulpe_editor_path(context->poulpe), IN_MODIFY) == -1)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot watch given file");
        pthread_exit(NULL);
    }

    while (context->open)
    {
        int32_t length = 0;
        char buffer[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
        const struct inotify_event *event;

        length = read(fd, buffer, sizeof(buffer));
        if (length == -1 && errno != EAGAIN) {
            PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot read inotify event buffer");
            pthread_exit(NULL);
        }

        for (char *ptr = buffer; ptr < buffer + length; ptr += sizeof(struct inotify_event) + event->len) 
        {
            event = (const struct inotify_event *) ptr;
            if (event->mask & IN_MODIFY)
                prisma_renderer_request_viewport_update(poulpe_editor_path(context->poulpe));
        }

        /* Sleep during 10 ms */
        struct timespec ts = {0};
        ts.tv_sec = 0;
        ts.tv_nsec = 10000000;
        nanosleep(&ts, NULL);
    }

    close(fd);

    pthread_exit(NULL);
}