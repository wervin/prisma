#ifndef PRISMA_COMPONENT_H
#define PRISMA_COMPONENT_H

#include <stdbool.h>

#include <cimgui.h>

#include "prisma/error.h"

#define PRISMA_COMPONENTS                      \
    X(PRISMA_COMPONENT_TYPE_EDITOR, 1, editor) \
    X(PRISMA_COMPONENT_TYPE_LOGGER, 2, logger) \
    X(PRISMA_COMPONENT_TYPE_MENU, 3, menu)     \
    X(PRISMA_COMPONENT_TYPE_VIEWPORT, 4, viewport)

enum prisma_component_type
{
#define X(__def, __id, __type) \
    __def = __id,

    PRISMA_COMPONENTS

#undef X
};

struct prisma_component
{
    enum prisma_component_type type;
    struct prisma_component *parent;
};

struct prisma_component * prisma_component_new(enum prisma_component_type type);
enum prisma_error prisma_component_draw(struct prisma_component *component);
void prisma_component_free(struct prisma_component *component);

#endif /* PRISMA_COMPONENT_H */