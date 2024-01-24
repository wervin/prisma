#ifndef PRISMA_COMPONENTS_VIEWPORT_H
#define PRISMA_COMPONENTS_VIEWPORT_H

#include "prisma/component.h"

struct prisma_ui;

struct prisma_viewport
{
    struct prisma_component base;
    struct prisma_ui *ui;
};

void prisma_viewport_set_ui(struct prisma_viewport *viewport, struct prisma_ui *ui);

#endif /* PRISMA_COMPONENTS_VIEWPORT_H */