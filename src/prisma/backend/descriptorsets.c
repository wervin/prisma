#include <stdlib.h>
#include <string.h>

#include "prisma/backend/descriptorsets.h"
#include "prisma/backend/descriptorsetlayout.h"
#include "prisma/backend/descriptorpool.h"
#include "prisma/backend/device.h"
#include "prisma/backend/buffers/uniformbuffer.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_descriptorsets_init(
    struct prisma_backend_descriptorsets *descriptorsets,
    struct prisma_backend_device *device,
    struct prisma_backend_descriptorpool *descriptorpool,
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout,
    struct prisma_backend_buffers_uniformbuffer *uniformbuffer,
    struct prisma_backend_descriptorsets_info *info)
{
    descriptorsets->max_frames_in_flight = info->max_frames_in_flight;
    descriptorsets->vk_descriptorsets = malloc(info->max_frames_in_flight * sizeof(VkDescriptorSet));
    if (descriptorsets->vk_descriptorsets == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    VkDescriptorSetLayout layouts[info->max_frames_in_flight];
    for (uint32_t i = 0; i < info->max_frames_in_flight; i++)
        layouts[i] = descriptorsetlayout->vk_descriptorsetlayout;

    VkDescriptorSetAllocateInfo allocate_info = {0};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = descriptorpool->vk_descriptorpool;
    allocate_info.descriptorSetCount = info->max_frames_in_flight;
    allocate_info.pSetLayouts = layouts;
    if (vkAllocateDescriptorSets(device->vk_device, &allocate_info, descriptorsets->vk_descriptorsets) != VK_SUCCESS) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to allocate descriptor sets");
        return PRISMA_ERROR_VK;
    }

    for (uint32_t i = 0; i < info->max_frames_in_flight; i++) {
        VkDescriptorBufferInfo frag_buffer_info = {0};
        frag_buffer_info.buffer = uniformbuffer->buffers[i].vk_buffer;
        frag_buffer_info.offset = 0;
        frag_buffer_info.range = uniformbuffer->size;

        VkWriteDescriptorSet frag_descriptor_write = {0};
        frag_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        frag_descriptor_write.dstSet = descriptorsets->vk_descriptorsets[i];
        frag_descriptor_write.dstBinding = 0;
        frag_descriptor_write.dstArrayElement = 0;
        frag_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        frag_descriptor_write.descriptorCount = 1;
        frag_descriptor_write.pBufferInfo = &frag_buffer_info;

        vkUpdateDescriptorSets(device->vk_device, 1, &frag_descriptor_write, 0, NULL);
    }
    
    return PRISMA_ERROR_NONE;
}

void prisma_backend_descriptorsets_destroy(struct prisma_backend_descriptorsets *descriptorsets)
{
    if (descriptorsets->vk_descriptorsets)
    {
        free(descriptorsets->vk_descriptorsets);
    }
}