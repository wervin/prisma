#ifndef PRISMA_COMPONENTS_EDITOR_H
#define PRISMA_COMPONENTS_EDITOR_H

#include <poulpe.h>

#include "prisma/component.h"
#include "prisma/error.h"

struct prisma_ui;

struct prisma_editor
{
    struct prisma_component base;
    struct poulpe_editor **editors;
    struct prisma_ui *ui;
};

void prisma_editor_set_ui(struct prisma_editor *editor, struct prisma_ui *ui);

enum prisma_error prisma_editor_open(struct prisma_editor *editor, const char *path);
enum prisma_error prisma_editor_close(struct prisma_editor *editor, const char *filename);

#endif /* PRISMA_COMPONENTS_EDITOR_H */