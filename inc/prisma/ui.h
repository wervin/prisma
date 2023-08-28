#ifndef PRISMA_UI_H
#define PRISMA_UI_H

#include <stdint.h>

#include "prisma/error.h"

struct prisma_ui_info
{
    uint8_t no;
};

struct prisma_ui
{
    uint8_t no;
};

/*
 * @brief Init Prisma ui
 * @param ui Prisma ui
 * @param info intial information about Prisma ui
 * @return Error if any
 */
enum prisma_error prisma_ui_init(struct prisma_ui *ui,
                                 struct prisma_ui_info *info);


/*
 * @brief Draw a frame from Prisma ui
 * @param ui Prisma ui
 * @return Error if any
 */
enum prisma_error prisma_ui_draw(struct prisma_ui *ui);

/*
 * @brief destroy Prisma ui
 * @param ui Prisma ui
 */
void prisma_ui_destroy(struct prisma_ui *ui);

#endif /* PRISMA_UI_H */