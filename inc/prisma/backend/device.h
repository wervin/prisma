#ifndef PRISMA_BACKEND_DEVICE_H
#define PRISMA_BACKEND_DEVICE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_instance;

struct prisma_backend_device_info
{
  VkPhysicalDeviceFeatures features_requested;
  const char** device_extensions;
  VkSurfaceFormatKHR vk_surface_format;
#ifndef NDEBUG
  const char** validation_layers;
#endif
};

struct prisma_backend_device
{ 
  VkPhysicalDevice vk_physical_device;
  VkDevice vk_device;
  VkPhysicalDeviceProperties vk_physical_device_properties;
  VkQueue vk_graphic_queue;
  uint32_t vk_graphic_queue_index;
  VkQueue vk_present_queue;
  uint32_t vk_present_queue_index;
};

enum prisma_error prisma_backend_device_init(
    struct prisma_backend_device *device,
    struct prisma_backend_instance *instance,
    struct prisma_backend_device_info *info);

void prisma_backend_device_destroy(struct prisma_backend_device *device);

void prisma_backend_device_wait_idle(struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_DEVICE_H */