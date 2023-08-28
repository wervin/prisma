#ifndef PRISMA_UI_H
#define PRISMA_UI_H

#include <stdint.h>

#include "prisma/error.h"

enum prisma_error prisma_ui_init(void);

enum prisma_error prisma_ui_draw(void);

void prisma_ui_destroy(void);

#endif /* PRISMA_UI_H */