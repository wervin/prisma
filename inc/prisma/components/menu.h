#ifndef PRISMA_COMPONENTS_MENU_H
#define PRISMA_COMPONENTS_MENU_H

#include "prisma/component.h"

struct prisma_ui;

struct prisma_menu
{
    struct prisma_component base;
    struct prisma_ui *ui;
};

void prisma_menu_set_ui(struct prisma_menu *menu, struct prisma_ui *ui);

#endif /* PRISMA_COMPONENTS_MENU_H */