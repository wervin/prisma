#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "prisma/backend/swapchain.h"
#include "prisma/backend/device.h"
#include "prisma/backend/instance.h"

#include "prisma/log.h"
#include "prisma/window.h"

static enum prisma_error _create_swapchain(struct prisma_backend_swapchain *swapchain,
                                           struct prisma_backend_instance *instance,
                                           struct prisma_backend_device *device,
                                           struct prisma_backend_swapchain_info *info);

static bool _support_desired_surface_format(struct prisma_backend_instance *instance,
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_swapchain_info *info);

static bool _support_desired_present_mode(struct prisma_backend_instance *instance,
                                          struct prisma_backend_device *device, 
                                          struct prisma_backend_swapchain_info *info);

static bool _support_desired_image_count(struct prisma_backend_instance *instance,
                                         struct prisma_backend_device *device,
                                         struct prisma_backend_swapchain_info *info);

static bool _support_desired_array_layer_count(struct prisma_backend_instance *instance,
                                               struct prisma_backend_device *device,
                                               struct prisma_backend_swapchain_info *info);

static VkExtent2D _get_current_extent(struct prisma_backend_instance *instance,
                                      struct prisma_backend_device *device);

enum prisma_error prisma_backend_swapchain_init(struct prisma_backend_swapchain *swapchain,
                                                struct prisma_backend_instance *instance,
                                                struct prisma_backend_device *device,
                                                struct prisma_backend_swapchain_info *info)
{
    enum prisma_error status;

    status = _create_swapchain(swapchain, instance, device, info);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }

    return PRISMA_ERROR_NONE; 
}

void prisma_backend_swapchain_destroy(struct prisma_backend_swapchain *swapchain, struct prisma_backend_device *device)
{
    if (swapchain->vk_image_views)
    {
        for (uint32_t i = 0; i < swapchain->image_count; i++)
            vkDestroyImageView(device->vk_device, swapchain->vk_image_views[i], NULL);
        free(swapchain->vk_image_views);
    }
    
    if (swapchain->vk_images)
        free(swapchain->vk_images);

    vkDestroySwapchainKHR(device->vk_device, swapchain->vk_swapchain, NULL);
}

static enum prisma_error _create_swapchain(struct prisma_backend_swapchain *swapchain,
                                           struct prisma_backend_instance *instance,
                                           struct prisma_backend_device *device,
                                           struct prisma_backend_swapchain_info *info)
{
    if (!_support_desired_surface_format(instance, device, info))
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "No swapchain support for desired surface format");
        return PRISMA_ERROR_VK;
    }

    if (!_support_desired_present_mode(instance, device, info))
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "No swapchain support for desired present mode");
        return PRISMA_ERROR_VK;
    }

    if (!_support_desired_image_count(instance, device, info))
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "No swapchain support for desired image count");
        return PRISMA_ERROR_VK;
    }

    if (!_support_desired_array_layer_count(instance, device, info))
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "No swapchain support for desired array layer count");
        return PRISMA_ERROR_VK;
    }

    swapchain->vk_extent = _get_current_extent(instance, device);

    VkSwapchainCreateInfoKHR swapchain_create_info = {0};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.flags = info->create_flags;
    swapchain_create_info.surface = instance->vk_surface;
    swapchain_create_info.minImageCount = info->image_count;
    swapchain_create_info.imageFormat = info->vk_surface_format.format;
    swapchain_create_info.imageColorSpace = info->vk_surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain->vk_extent;
    swapchain_create_info.imageArrayLayers = info->array_layer_count;
    swapchain_create_info.imageUsage = info->image_usage_flags;

    uint32_t queue_family_indices[] = {device->vk_graphic_queue_index, device->vk_present_queue_index};
    if (device->vk_graphic_queue_index != device->vk_present_queue_index) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->vk_physical_device, 
                                              instance->vk_surface,
                                              &capabilities);

    swapchain_create_info.preTransform = info->transform ? info->transform : capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = info->composite_alpha;
    swapchain_create_info.presentMode = info->vk_present_mode;
    swapchain_create_info.clipped = info->clipped;
    swapchain_create_info.oldSwapchain = NULL;
    if (vkCreateSwapchainKHR(device->vk_device, &swapchain_create_info, NULL, &swapchain->vk_swapchain) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create swap chain");
        return PRISMA_ERROR_VK;
    }

    vkGetSwapchainImagesKHR(device->vk_device, swapchain->vk_swapchain, &swapchain->image_count, NULL);

    swapchain->vk_images = malloc(swapchain->image_count * sizeof(VkImage));
    if (swapchain->vk_images == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    vkGetSwapchainImagesKHR(device->vk_device, swapchain->vk_swapchain, &swapchain->image_count, swapchain->vk_images);

    swapchain->vk_image_views = malloc(swapchain->image_count * sizeof(VkImageView));
    if (swapchain->vk_image_views == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    for (uint32_t i = 0; i < swapchain->image_count; i++)
    {
        VkImageViewCreateInfo imageview_createinfo = {0};
        imageview_createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageview_createinfo.image = swapchain->vk_images[i];
        imageview_createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageview_createinfo.format = info->vk_surface_format.format;
        imageview_createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageview_createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageview_createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageview_createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageview_createinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageview_createinfo.subresourceRange.baseMipLevel = 0;
        imageview_createinfo.subresourceRange.levelCount = 1;
        imageview_createinfo.subresourceRange.baseArrayLayer = 0;
        imageview_createinfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device->vk_device, &imageview_createinfo, NULL, &swapchain->vk_image_views[i]) != VK_SUCCESS)
        {
            PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create image view");
            return PRISMA_ERROR_VK;
        }
    }

    return PRISMA_ERROR_NONE;
}

static bool _support_desired_surface_format(struct prisma_backend_instance *instance,
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_swapchain_info *info)
{
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->vk_physical_device, 
                                         instance->vk_surface, 
                                         &format_count, 
                                         NULL);

    VkSurfaceFormatKHR formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->vk_physical_device, 
                                         instance->vk_surface,
                                         &format_count, 
                                         formats);

    for (uint32_t i = 0; i < format_count; i++)
    {
        if ((formats[i].colorSpace == info->vk_surface_format.colorSpace) &&
            (formats[i].format == info->vk_surface_format.format))
            return true;
    }
    
    return false;
}

static bool _support_desired_present_mode(struct prisma_backend_instance *instance,
                                          struct prisma_backend_device *device, 
                                          struct prisma_backend_swapchain_info *info)
{
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->vk_physical_device, 
                                             instance->vk_surface, 
                                             &present_mode_count, 
                                             NULL);
    
    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->vk_physical_device, 
                                              instance->vk_surface,
                                              &present_mode_count,
                                              present_modes);
    
    for (uint32_t i = 0; i < present_mode_count; i++)
    {
        if (present_modes[i] == info->vk_present_mode)
            return true;
    }
    
    return false;
}

static bool _support_desired_image_count(struct prisma_backend_instance *instance,
                                         struct prisma_backend_device *device,
                                         struct prisma_backend_swapchain_info *info)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->vk_physical_device, 
                                              instance->vk_surface,
                                              &capabilities);
    bool supported_min_image_count = info->image_count >= capabilities.minImageCount;
    bool supported_max_image_count = !capabilities.maxImageCount || info->image_count <= capabilities.maxImageCount;
    return supported_max_image_count && supported_min_image_count;
}

static VkExtent2D _get_current_extent(struct prisma_backend_instance *instance,
                                      struct prisma_backend_device *device)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->vk_physical_device, 
                                              instance->vk_surface,
                                              &capabilities);

    if (capabilities.currentExtent.width != UINT_MAX) 
    {
        return capabilities.currentExtent;
    } 
    else 
    {
        uint32_t width, height;
        prisma_window_get_extent(&width, &height);
        VkExtent2D extent = {.height = height, .width = width};
        extent.width = fmax(capabilities.minImageExtent.width,
                                  fmin(capabilities.maxImageExtent.width, extent.width));
        extent.height = fmax(capabilities.minImageExtent.height,
                                   fmin(capabilities.maxImageExtent.height, extent.height));
        return extent;
    }
}

static bool _support_desired_array_layer_count(struct prisma_backend_instance *instance,
                                               struct prisma_backend_device *device,
                                               struct prisma_backend_swapchain_info *info)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->vk_physical_device, 
                                              instance->vk_surface,
                                              &capabilities);
    return info->array_layer_count <= capabilities.maxImageArrayLayers;
}