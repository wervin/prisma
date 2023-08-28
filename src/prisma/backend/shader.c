#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "prisma/backend/shader.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_shader_init(struct prisma_backend_shader *shader,
                                             struct prisma_backend_device *device,
                                             struct prisma_backend_shader_info *info)
{
    uint32_t size;
    uint8_t *buffer;

    FILE* fd = fopen(info->path, "rb");
    if (!fd) 
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_FILE, info->path);
        return PRISMA_ERROR_FILE;
    }

    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    
    buffer = malloc(size * sizeof(uint8_t));
    if (buffer == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

	rewind(fd);
	fread(buffer, 1, size, fd);
    fclose(fd);

    VkShaderModuleCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = size;
    create_info.pCode = (uint32_t *) buffer;
    if (vkCreateShaderModule(device->vk_device, &create_info, NULL, &shader->vk_shader_module) != VK_SUCCESS) 
    {
        free(buffer);
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create shader module");
        return PRISMA_ERROR_VK;
    }

    free(buffer);
    return PRISMA_ERROR_NONE;
}

void prisma_backend_shader_destroy(struct prisma_backend_shader *shader, struct prisma_backend_device *device)
{
    vkDestroyShaderModule(device->vk_device, shader->vk_shader_module, NULL);
}
