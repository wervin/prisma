#include "prisma/backend/pipelinelayout.h"
#include "prisma/backend/device.h"
#include "prisma/backend/descriptorsetlayout.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_pipelinelayout_init(
    struct prisma_backend_pipelinelayout *pipelinelayout, 
    struct prisma_backend_device *device,
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout)
{
    VkPipelineLayoutCreateInfo pipelinelayout_info = {0};
    pipelinelayout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelinelayout_info.flags = 0;
    pipelinelayout_info.setLayoutCount = 1;
    pipelinelayout_info.pSetLayouts = &descriptorsetlayout->vk_descriptorsetlayout;
    pipelinelayout_info.pushConstantRangeCount = 0;
    pipelinelayout_info.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(device->vk_device, &pipelinelayout_info, NULL, &pipelinelayout->vk_pipelinelayout) !=
        VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create pipeline layout");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_pipelinelayout_destroy(struct prisma_backend_pipelinelayout *pipelinelayout, struct prisma_backend_device *device)
{
    vkDestroyPipelineLayout(device->vk_device, pipelinelayout->vk_pipelinelayout, NULL);
}
