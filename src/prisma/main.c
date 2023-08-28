#include "prisma/application.h"
#include "prisma/error.h"

int main(void)
{
    enum prisma_error error = PRISMA_ERROR_NONE;
    struct prisma_application application = {0};
    struct prisma_application_info info = {
        .default_width = 800,
        .default_height = 600,
        .fullscreen = true,

        .application_name = "Prisma",
        .application_version_major = 1,
        .application_version_minor = 0,
        .application_version_revision = 0,

        .vertex_shader_path = "default.vert.spv",
        .frag_shader_path = "default.frag.spv",

        .max_frames_in_flight = 2,
        .features_requested = {
            .samplerAnisotropy = VK_TRUE,
        },
        .vk_surface_format = (VkSurfaceFormatKHR) {.format=VK_FORMAT_B8G8R8A8_SRGB, .colorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        .array_layer_count = 1,
        .clipped = VK_TRUE,
        .composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .create_flags = 0,
        .image_count = 3,
        .image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .transform = 0,
        .vk_present_mode = VK_PRESENT_MODE_FIFO_KHR,
        .sample_count = VK_SAMPLE_COUNT_1_BIT,
        .device_extensions = (const char *[]) {"VK_KHR_swapchain"},
#ifndef NDEBUG
        .validation_layers = (const char *[]) {"VK_LAYER_KHRONOS_validation"},
        .debug_extensions = (const char *[]) {"VK_EXT_debug_utils"}
#endif
    };

    error = prisma_application_init(&application, &info);
    if (error != PRISMA_ERROR_NONE)
        goto clean;

    error = prisma_application_run(&application);
    if (error != PRISMA_ERROR_NONE)
        goto clean;

clean:
    prisma_application_destroy(&application);
    return error;
}