#ifndef PRISMA_BACKEND_SWAPCHAIN_H
#define PRISMA_BACKEND_SWAPCHAIN_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_instance;
struct prisma_backend_device;

struct prisma_backend_swapchain_info
{
  VkSurfaceFormatKHR vk_surface_format;
  VkPresentModeKHR  vk_present_mode;
  uint32_t image_count;
  uint32_t array_layer_count;
  VkSurfaceTransformFlagBitsKHR transform;
  VkSwapchainCreateFlagBitsKHR create_flags;
  VkImageUsageFlags image_usage_flags;
  VkCompositeAlphaFlagBitsKHR composite_alpha;
  VkBool32 clipped;
};

struct prisma_backend_swapchain
{
  VkSwapchainKHR vk_swapchain;
  VkExtent2D vk_extent;
  VkImage* vk_images;
  VkImageView* vk_image_views;
  uint32_t image_count;
};

enum prisma_error prisma_backend_swapchain_init(
    struct prisma_backend_swapchain *swapchain,
    struct prisma_backend_instance *instance,
    struct prisma_backend_device *device,
    struct prisma_backend_swapchain_info *info);

void prisma_backend_swapchain_destroy(
    struct prisma_backend_swapchain *swapchain,
    struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_SWAPCHAIN_H */