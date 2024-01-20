#include "prisma/component.h"

#include "prisma/log.h"

#define X(__def, __id, __type)                                                                                                           \
    struct prisma_##__type;                                                                                                              \
    extern struct prisma_##__type *prisma_##__type##_new(void);                                                                          \
    extern enum prisma_error prisma_##__type##_draw(struct prisma_##__type *component);                                                  \
    extern void prisma_##__type##_free(struct prisma_##__type *component);

PRISMA_COMPONENTS

#undef X

struct prisma_component *prisma_component_new(enum prisma_component_type type)
{
    struct prisma_component *component;

    switch (type)
    {
#define X(__def, __id, __type)                                          \
    case __id:                                                          \
        component = (struct prisma_component *)prisma_##__type##_new(); \
        component->parent = NULL;                                       \
        component->type = __id;                                         \
        return component;

        PRISMA_COMPONENTS

#undef X

    default:
        PRISMA_LOG_ERROR(PRISMA_ERROR_UNKNOWN, "Unknown component type");
        return NULL;
    }
}

enum prisma_error prisma_component_draw(struct prisma_component *component)
{
    switch (component->type)
    {
#define X(__def, __id, __type) \
    case __id:                 \
        return prisma_##__type##_draw((struct prisma_##__type *)component);

        PRISMA_COMPONENTS

#undef X

    default:
        PRISMA_LOG_ERROR(PRISMA_ERROR_UNKNOWN, "Unknown component type");
        return PRISMA_ERROR_UNKNOWN;
    }
}

void prisma_component_free(struct prisma_component *component)
{
    switch (component->type)
    {
#define X(__def, __id, __type)                                       \
    case __id:                                                       \
        prisma_##__type##_free((struct prisma_##__type *)component); \
        break;

        PRISMA_COMPONENTS

#undef X

    default:
        PRISMA_LOG_ERROR(PRISMA_ERROR_UNKNOWN, "Unknown component type");
    }
}