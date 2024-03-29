#ifndef PRISMA_WINDOW_H
#define PRISMA_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

#include "prisma/error.h"

struct prisma_window_info
{
    uint32_t default_width;
    uint32_t default_height;
    bool fullscreen;
    const char *application_name;
};

enum prisma_error prisma_window_init(struct prisma_window_info *info);

void prisma_window_show(void);

void prisma_window_get_extent(int *width, int *height);

bool prisma_window_should_close(void);

void prisma_window_poll_events(void);

void prisma_window_wait_events(void);

enum prisma_error prisma_window_create_surface(void * instance, void *surface);

const char** prisma_window_get_required_extensions(uint32_t* count);

void prisma_window_destroy(void);

enum prisma_error primsa_window_init_ui(void);

void prisma_window_draw_ui(void);

void prisma_window_destroy_ui(void);

#endif /* PRISMA_WINDOW_H */