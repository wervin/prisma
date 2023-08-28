#ifndef PRISMA_RENDERER_H
#define PRISMA_RENDERER_H

#include <stdint.h>

#include "prisma/error.h"

enum prisma_error prisma_renderer_init(void);

enum prisma_error prisma_renderer_draw(void);

void prisma_renderer_destroy(void);

#endif /* PRISMA_RENDERER_H */