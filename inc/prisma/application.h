#ifndef PRISMA_APPLICATION_H
#define PRISMA_APPLICATION_H

#include <stdint.h>

#include "prisma/error.h"
#include "prisma/renderer.h"
#include "prisma/ui.h"
#include "prisma/window.h"

struct prisma_application_info
{
    uint32_t default_width;
    uint32_t default_height;
    bool fullscreen;
    
    const char *application_name;
    uint32_t application_version_major;
    uint32_t application_version_minor;
    uint32_t application_version_revision;

    const char *vertex_shader_path;
    const char *frag_shader_path;

    uint32_t max_frames_in_flight;
    VkPhysicalDeviceFeatures features_requested;
    const char **device_extensions;
    VkSurfaceFormatKHR vk_surface_format;
    VkPresentModeKHR  vk_present_mode;
    uint32_t image_count;
    uint32_t array_layer_count;
    VkSurfaceTransformFlagBitsKHR transform;
    VkSwapchainCreateFlagBitsKHR create_flags;
    VkImageUsageFlags image_usage_flags;
    VkCompositeAlphaFlagBitsKHR composite_alpha;
    VkBool32 clipped;
    VkSampleCountFlagBits sample_count;
    
#ifndef NDEBUG
  const char** validation_layers;
  const char** debug_extensions;
#endif
};

struct prisma_application
{
    struct prisma_renderer renderer;
    struct prisma_ui ui;
    struct prisma_window window;
};

/*
 * @brief Init Prisma application
 * @param application Prisma application
 * @param info intial information about Prisma application
 * @return Error if any
 */
enum prisma_error prisma_application_init(struct prisma_application *application,
                                          struct prisma_application_info *info);

/*
 * @brief run Prisma application
 * @param application Prisma application
 * @return Error if any
 */
enum prisma_error prisma_application_run(struct prisma_application *application);

/*
 * @brief destroy Prisma application
 * @param application Prisma application
 */
void prisma_application_destroy(struct prisma_application *application);

#endif /* PRISMA_APPLICATION_H */