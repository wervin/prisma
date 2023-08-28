#ifndef PRISMA_APPLICATION_H
#define PRISMA_APPLICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "prisma/error.h"

struct prisma_application_info
{
    uint32_t default_width;
    uint32_t default_height;
    bool fullscreen;
    
    const char *application_name;
    uint32_t application_version_major;
    uint32_t application_version_minor;
    uint32_t application_version_revision;
};

/*
 * @brief Init Prisma application
 * @param application Prisma application
 * @param info intial information about Prisma application
 * @return Error if any
 */
enum prisma_error prisma_application_init(struct prisma_application_info *info);

/*
 * @brief run Prisma application
 * @param application Prisma application
 * @return Error if any
 */
enum prisma_error prisma_application_run(void);

/*
 * @brief destroy Prisma application
 * @param application Prisma application
 */
void prisma_application_destroy(void);

#endif /* PRISMA_APPLICATION_H */