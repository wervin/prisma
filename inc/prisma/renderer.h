#ifndef PRISMA_RENDERER_H
#define PRISMA_RENDERER_H

#include <stdint.h>

#include "prisma/error.h"

struct prisma_renderer_info
{
    uint8_t no;
};

struct prisma_renderer
{
    uint8_t no;
};

/*
 * @brief Init Prisma renderer
 * @param renderer Prisma renderer
 * @param info intial information about Prisma renderer
 * @return Error if any
 */
enum prisma_error prisma_renderer_init(struct prisma_renderer *renderer,
                                       struct prisma_renderer_info *info);

/*
 * @brief destroy Prisma renderer
 * @param renderer Prisma renderer
 */
void prisma_renderer_destroy(struct prisma_renderer *renderer);

#endif /* PRISMA_RENDERER_H */