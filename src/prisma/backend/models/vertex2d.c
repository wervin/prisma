#include <stddef.h>
#include <stdalign.h>

#include "prisma/backend/models/vertex2d.h"

struct prisma_backend_models_vertex2d_input prisma_backend_models_vertex2d_get_input(void)
{
    struct prisma_backend_models_vertex2d_input vertex_input = {
        .attributes = {
            {
                .binding = 0,
                .location = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(struct prisma_backend_models_vertex2d, position)
            }
        },
        .binding = {
            .binding = 0,
            .stride = sizeof(struct prisma_backend_models_vertex2d),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };

    return vertex_input;
}