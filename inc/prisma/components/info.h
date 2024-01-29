#ifndef PRISMA_COMPONENTS_INFO_H
#define PRISMA_COMPONENTS_INFO_H

#include "prisma/component.h"

struct prisma_ui;

struct prisma_info
{
    struct prisma_component base;
    struct prisma_ui *ui;
};

void prisma_info_set_ui(struct prisma_info *info, struct prisma_ui *ui);

#endif /* PRISMA_COMPONENTS_INFO_H */