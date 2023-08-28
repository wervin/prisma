#ifndef PRISMA_BACKEND_INSTANCE_H
#define PRISMA_BACKEND_INSTANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_window;

struct prisma_backend_instance_info
{
  const char *application_name;
  uint32_t application_version_major;
  uint32_t application_version_minor;
  uint32_t application_version_revision;

#ifndef NDEBUG
  const char** validation_layers;
  const char** debug_extensions;
#endif
};

struct prisma_backend_instance
{
  VkInstance vk_instance;
  VkSurfaceKHR vk_surface;
#ifndef NDEBUG
  VkDebugUtilsMessengerEXT vk_debug_messenger;
#endif
};

enum prisma_error prisma_backend_instance_init(
    struct prisma_backend_instance *instance,
    struct prisma_window *window,
    struct prisma_backend_instance_info *info);

void prisma_backend_instance_destroy(struct prisma_backend_instance *instance);

#endif /* PRISMA_BACKEND_INSTANCE_H */