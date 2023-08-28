#include "prisma/application.h"
#include "prisma/error.h"

int main(void)
{
    enum prisma_error error = PRISMA_ERROR_NONE;

    struct prisma_application_info info = {
        .default_width = 800,
        .default_height = 600,
        .fullscreen = true,

        .application_name = "Prisma",
        .application_version_major = 1,
        .application_version_minor = 0,
        .application_version_revision = 0
};

    error = prisma_application_init(&info);
    if (error != PRISMA_ERROR_NONE)
        goto clean;

    error = prisma_application_run();
    if (error != PRISMA_ERROR_NONE)
        goto clean;

clean:
    prisma_application_destroy();
    return error;
}