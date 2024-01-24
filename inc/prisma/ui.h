#ifndef PRISMA_UI_H
#define PRISMA_UI_H

#include <stdint.h>

#include "prisma/error.h"

struct prisma_editor;
struct prisma_logger;
struct prisma_menu;
struct prisma_viewport;

struct prisma_ui {
    struct prisma_editor *editor;
    struct prisma_logger *logger;
    struct prisma_menu *menu;
    struct prisma_viewport *viewport;
};

enum prisma_error prisma_ui_init(void);

void prisma_ui_draw(void);

void prisma_ui_destroy(void);

#endif /* PRISMA_UI_H */