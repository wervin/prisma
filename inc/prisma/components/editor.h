#ifndef PRISMA_COMPONENTS_EDITOR_H
#define PRISMA_COMPONENTS_EDITOR_H

#include <pthread.h>
#include <stdbool.h>

#include "prisma/component.h"
#include "prisma/error.h"

struct prisma_ui;
struct poulpe_editor;

struct prisma_editor_context
{
    struct poulpe_editor *poulpe;
    bool temporary;
    bool open;
    pthread_t thread_id;
};

struct prisma_editor
{
    struct prisma_component base;
    struct prisma_editor_context **contexts;
    struct prisma_editor_context *current;
    struct prisma_ui *ui;
    pthread_mutex_t mutex;
};

void prisma_editor_set_ui(struct prisma_editor *editor, struct prisma_ui *ui);
const char *prisma_editor_context_filename(struct prisma_editor_context *view);
enum prisma_error prisma_editor_open_file(struct prisma_editor *editor, const char *path);
enum prisma_error prisma_editor_new_file(struct prisma_editor *editor);
enum prisma_error prisma_editor_copy_file(struct prisma_editor *editor, const char *path);
enum prisma_error prisma_editor_close_file(struct prisma_editor *editor);
enum prisma_error prisma_editor_save_as_file(struct prisma_editor *editor, const char *path);

#endif /* PRISMA_COMPONENTS_EDITOR_H */