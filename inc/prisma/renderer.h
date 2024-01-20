#ifndef PRISMA_RENDERER_H
#define PRISMA_RENDERER_H

#include <stdint.h>

#include "prisma/error.h"

enum prisma_error prisma_renderer_init(void);

enum prisma_error prisma_renderer_draw(void);

void prisma_renderer_wait_idle(void);

void prisma_renderer_destroy(void);

enum prisma_error prisma_renderer_init_ui(void);

void prisma_renderer_refresh_ui(void);

void prisma_renderer_draw_ui(void);

void prisma_renderer_destroy_ui(void);

enum prisma_error prisma_renderer_init_viewport(void);

void prisma_renderer_draw_viewport(void);

void prisma_renderer_destroy_viewport(void);

#endif /* PRISMA_RENDERER_H */