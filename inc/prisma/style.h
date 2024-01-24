#ifndef PRISMA_STYLE_H
#define PRISMA_STYLE_H

#include "prisma/error.h"

typedef struct ImFont ImFont;

struct prisma_style
{
    ImFont *icon_font;
    ImFont *large_font;
    ImFont *small_font;
};

void prisma_style_init(void);

extern struct prisma_style prisma_style;

#endif /* PRISMA_STYLE_H */