#ifndef PRISMA_COMPONENTS_EDITOR_H
#define PRISMA_COMPONENTS_EDITOR_H

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
};

struct prisma_editor
{
    struct prisma_component base;
    struct prisma_editor_context **contexts;
    struct prisma_editor_context *current;
    struct prisma_ui *ui;
};

void prisma_editor_set_ui(struct prisma_editor *editor, struct prisma_ui *ui);
const char *prisma_editor_context_filename(struct prisma_editor_context *view);
enum prisma_error prisma_editor_open(struct prisma_editor *editor, const char *path);

#endif /* PRISMA_COMPONENTS_EDITOR_H */