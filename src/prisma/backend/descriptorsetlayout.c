#include "prisma/backend/descriptorsetlayout.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_descriptorsetlayout_init(
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout,
    struct prisma_backend_device *device)
{
    VkDescriptorSetLayoutBinding ubo_layoutbinding_info = {0};
    ubo_layoutbinding_info.binding = 0;
    ubo_layoutbinding_info.descriptorCount = 1;
    ubo_layoutbinding_info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layoutbinding_info.pImmutableSamplers = NULL;
    ubo_layoutbinding_info.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layout_info = {0};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &ubo_layoutbinding_info;
    if (vkCreateDescriptorSetLayout(device->vk_device, &layout_info, NULL, &descriptorsetlayout->vk_descriptorsetlayout) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create descriptor set layout");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_descriptorsetlayout_destroy(
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout,
    struct prisma_backend_device *device)
{
    vkDestroyDescriptorSetLayout(device->vk_device, descriptorsetlayout->vk_descriptorsetlayout, NULL);
}
