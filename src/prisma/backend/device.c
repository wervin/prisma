#include <string.h>
#include <stdbool.h>

#include "prisma/backend/device.h"
#include "prisma/backend/instance.h"

#include "prisma/log.h"

static enum prisma_error _create_logical_device(struct prisma_backend_device *device,
                                                struct prisma_backend_instance *instance,
                                                struct prisma_backend_device_info *info);

static bool _is_physical_device_suitable(struct prisma_backend_instance *instance,
                                         VkPhysicalDevice vk_physical_device,
                                         struct prisma_backend_device_info *info);

static enum prisma_error _pick_physical_device(struct prisma_backend_device *device,
                                               struct prisma_backend_instance *instance,
                                               struct prisma_backend_device_info *info);

static bool _check_device_extension_support(VkPhysicalDevice physical_device,
                                            const char **desired_extensions,
                                            uint32_t desired_extension_count);
                                            
static bool _supports_features(VkPhysicalDeviceFeatures supported, VkPhysicalDeviceFeatures requested);
static int32_t _get_backend_queue_index(VkPhysicalDevice physical_device);
static int32_t _get_present_queue_index(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

enum prisma_error prisma_backend_device_init(struct prisma_backend_device *device,
                                             struct prisma_backend_instance *instance,
                                             struct prisma_backend_device_info *info)
{   
    enum prisma_error status;

    status = _pick_physical_device(device, instance, info);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }
    status = _create_logical_device(device, instance, info);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    };

    return PRISMA_ERROR_NONE;
}

void prisma_backend_device_destroy(struct prisma_backend_device *device)
{
    vkDestroyDevice(device->vk_device, NULL);
}

void prisma_backend_device_wait_idle(struct prisma_backend_device *device)
{
    vkDeviceWaitIdle(device->vk_device);
}

static enum prisma_error _create_logical_device(struct prisma_backend_device *device,
                                                struct prisma_backend_instance *instance,
                                                struct prisma_backend_device_info *info)
{
    int32_t backend_queue, present_queue;
    backend_queue = _get_backend_queue_index(device->vk_physical_device);
    present_queue = _get_present_queue_index(device->vk_physical_device, instance->vk_surface);

    int32_t queues[] = {backend_queue, present_queue};
    int32_t nb_queue = backend_queue != present_queue ? 2 : 1;
    VkDeviceQueueCreateInfo vk_queue_create_info[nb_queue];
    float queue_priority = 1.0f;
    for (int32_t i = 0; i < nb_queue; i++)
    {
        vk_queue_create_info[i] = (VkDeviceQueueCreateInfo) {0};
        vk_queue_create_info[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        vk_queue_create_info[i].queueFamilyIndex = queues[i];
        vk_queue_create_info[i].queueCount = 1;
        vk_queue_create_info[i].pQueuePriorities = &queue_priority;
    }

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = nb_queue;
    create_info.pQueueCreateInfos = (const VkDeviceQueueCreateInfo *) &vk_queue_create_info;
    create_info.pEnabledFeatures = &info->features_requested;
    create_info.enabledExtensionCount = sizeof(info->device_extensions) / sizeof(char*);
    create_info.ppEnabledExtensionNames = info->device_extensions;

#ifdef NDEBUG
    create_info.enabledLayerCount = 0;
#else
    create_info.enabledLayerCount = sizeof(info->validation_layers) / sizeof(char*);
    create_info.ppEnabledLayerNames = info->validation_layers;
#endif

    if (vkCreateDevice(device->vk_physical_device, &create_info, NULL, &device->vk_device) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create logical device");
        return PRISMA_ERROR_VK;
    }

    vkGetDeviceQueue(device->vk_device, backend_queue, 0, &device->vk_graphic_queue);
    vkGetDeviceQueue(device->vk_device, present_queue, 0, &device->vk_present_queue);
    device->vk_graphic_queue_index = backend_queue;
    device->vk_present_queue_index = present_queue;

    return PRISMA_ERROR_NONE;
}

static enum prisma_error _pick_physical_device(struct prisma_backend_device *device,
                                               struct prisma_backend_instance *instance,
                                               struct prisma_backend_device_info *info)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance->vk_instance, &device_count, NULL);
    if (!device_count)
    {
       PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to find GPUs with Vulkan support");
       return PRISMA_ERROR_VK;
    }

    PRISMA_LOG_DEBUG("Device count: %u\n", device_count);
    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(instance->vk_instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++)
    {
        if (_is_physical_device_suitable(instance, devices[i], info) == true)
        {
            device->vk_physical_device = devices[i];
            break;
        }
    }

    if (device->vk_physical_device == VK_NULL_HANDLE)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to find a suitable GPU");
        return PRISMA_ERROR_VK;
    }

    vkGetPhysicalDeviceProperties(device->vk_physical_device, &device->vk_physical_device_properties);
    PRISMA_LOG_DEBUG("Physical device: %s\n", device->vk_physical_device_properties.deviceName);

    return PRISMA_ERROR_NONE;
}

static bool _is_physical_device_suitable(struct prisma_backend_instance *instance,
                                         VkPhysicalDevice vk_physical_device,
                                         struct prisma_backend_device_info *info)
{
    if (_get_backend_queue_index(vk_physical_device) < 0)
        return false;

    if (_get_present_queue_index(vk_physical_device, instance->vk_surface) < 0)
        return false;

    if (!_check_device_extension_support(vk_physical_device, info->device_extensions, sizeof(info->device_extensions) / sizeof(char*)))
        return false;

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, instance->vk_surface, &format_count, NULL);
    if (!format_count)
        return false;

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, instance->vk_surface, &present_mode_count, NULL);
    if (!present_mode_count)
        return false;

    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(vk_physical_device, info->vk_surface_format.format, &format_properties);
    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
        return false;

    vkGetPhysicalDeviceFormatProperties(vk_physical_device, VK_FORMAT_R8G8B8A8_UNORM, &format_properties);
    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
        return false;

    VkPhysicalDeviceFeatures supported;
    vkGetPhysicalDeviceFeatures(vk_physical_device, &supported);
    if (!_supports_features(supported, info->features_requested))
        return false;

    return true;
}

static bool _check_device_extension_support(VkPhysicalDevice physical_device,
                                            const char **desired_extensions,
                                            uint32_t desired_extension_count)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, NULL);
    VkExtensionProperties available_extensions[extension_count];
    vkEnumerateDeviceExtensionProperties(
        physical_device,
        NULL,
        &extension_count,
        available_extensions);

    for (uint32_t i = 0; i < desired_extension_count; i++)
    {
        uint32_t j = 0;
        while (j < extension_count && strcmp(desired_extensions[i], available_extensions[j].extensionName) != 0)
            j++;
        if (strcmp(desired_extensions[i], available_extensions[j].extensionName) == 0)
            PRISMA_LOG_DEBUG("Extension device '%s' found\n", desired_extensions[i]);
        else
            return false;
    }

    return true;
}

static bool _supports_features(VkPhysicalDeviceFeatures supported, VkPhysicalDeviceFeatures requested)
{
    if (requested.robustBufferAccess && !supported.robustBufferAccess) return false;
    if (requested.fullDrawIndexUint32 && !supported.fullDrawIndexUint32) return false;
    if (requested.imageCubeArray && !supported.imageCubeArray) return false;
    if (requested.independentBlend && !supported.independentBlend) return false;
    if (requested.geometryShader && !supported.geometryShader) return false;
    if (requested.tessellationShader && !supported.tessellationShader) return false;
    if (requested.sampleRateShading && !supported.sampleRateShading) return false;
    if (requested.dualSrcBlend && !supported.dualSrcBlend) return false;
    if (requested.logicOp && !supported.logicOp) return false;
    if (requested.multiDrawIndirect && !supported.multiDrawIndirect) return false;
    if (requested.drawIndirectFirstInstance && !supported.drawIndirectFirstInstance) return false;
    if (requested.depthClamp && !supported.depthClamp) return false;
    if (requested.depthBiasClamp && !supported.depthBiasClamp) return false;
    if (requested.fillModeNonSolid && !supported.fillModeNonSolid) return false;
    if (requested.depthBounds && !supported.depthBounds) return false;
    if (requested.wideLines && !supported.wideLines) return false;
    if (requested.largePoints && !supported.largePoints) return false;
    if (requested.alphaToOne && !supported.alphaToOne) return false;
    if (requested.multiViewport && !supported.multiViewport) return false;
    if (requested.samplerAnisotropy && !supported.samplerAnisotropy) return false;
    if (requested.textureCompressionETC2 && !supported.textureCompressionETC2) return false;
    if (requested.textureCompressionASTC_LDR && !supported.textureCompressionASTC_LDR) return false;
    if (requested.textureCompressionBC && !supported.textureCompressionBC) return false;
    if (requested.occlusionQueryPrecise && !supported.occlusionQueryPrecise) return false;
    if (requested.pipelineStatisticsQuery && !supported.pipelineStatisticsQuery) return false;
    if (requested.vertexPipelineStoresAndAtomics && !supported.vertexPipelineStoresAndAtomics) return false;
    if (requested.fragmentStoresAndAtomics && !supported.fragmentStoresAndAtomics) return false;
    if (requested.shaderTessellationAndGeometryPointSize && !supported.shaderTessellationAndGeometryPointSize) return false;
    if (requested.shaderImageGatherExtended && !supported.shaderImageGatherExtended) return false;
    if (requested.shaderStorageImageExtendedFormats && !supported.shaderStorageImageExtendedFormats) return false;
    if (requested.shaderStorageImageMultisample && !supported.shaderStorageImageMultisample) return false;
    if (requested.shaderStorageImageReadWithoutFormat && !supported.shaderStorageImageReadWithoutFormat) return false;
    if (requested.shaderStorageImageWriteWithoutFormat && !supported.shaderStorageImageWriteWithoutFormat) return false;
    if (requested.shaderUniformBufferArrayDynamicIndexing && !supported.shaderUniformBufferArrayDynamicIndexing) return false;
    if (requested.shaderSampledImageArrayDynamicIndexing && !supported.shaderSampledImageArrayDynamicIndexing) return false;
    if (requested.shaderStorageBufferArrayDynamicIndexing && !supported.shaderStorageBufferArrayDynamicIndexing) return false;
    if (requested.shaderStorageImageArrayDynamicIndexing && !supported.shaderStorageImageArrayDynamicIndexing) return false;
    if (requested.shaderClipDistance && !supported.shaderClipDistance) return false;
    if (requested.shaderCullDistance && !supported.shaderCullDistance) return false;
    if (requested.shaderFloat64 && !supported.shaderFloat64) return false;
    if (requested.shaderInt64 && !supported.shaderInt64) return false;
    if (requested.shaderInt16 && !supported.shaderInt16) return false;
    if (requested.shaderResourceResidency && !supported.shaderResourceResidency) return false;
    if (requested.shaderResourceMinLod && !supported.shaderResourceMinLod) return false;
    if (requested.sparseBinding && !supported.sparseBinding) return false;
    if (requested.sparseResidencyBuffer && !supported.sparseResidencyBuffer) return false;
    if (requested.sparseResidencyImage2D && !supported.sparseResidencyImage2D) return false;
    if (requested.sparseResidencyImage3D && !supported.sparseResidencyImage3D) return false;
    if (requested.sparseResidency2Samples && !supported.sparseResidency2Samples) return false;
    if (requested.sparseResidency4Samples && !supported.sparseResidency4Samples) return false;
    if (requested.sparseResidency8Samples && !supported.sparseResidency8Samples) return false;
    if (requested.sparseResidency16Samples && !supported.sparseResidency16Samples) return false;
    if (requested.sparseResidencyAliased && !supported.sparseResidencyAliased) return false;
    if (requested.variableMultisampleRate && !supported.variableMultisampleRate) return false;
    if (requested.inheritedQueries && !supported.inheritedQueries) return false;
	return true;
}

static int32_t _get_backend_queue_index(VkPhysicalDevice physical_device)
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++)
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) return i;
    return -1;
}

static int32_t _get_present_queue_index(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++)
    {
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        if (present_support == VK_TRUE) return i;
    }
    return -1;
}