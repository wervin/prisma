#ifndef PRISMA_ERROR_H
#define PRISMA_ERROR_H

#define PRISMA_ERROR                           \
    X(PRISMA_ERROR_NONE, 0, "No error")        \
    X(PRISMA_ERROR_VALUE, -1, "Value error")   \
    X(PRISMA_ERROR_FILE, -2, "File error")     \
    X(PRISMA_ERROR_MEMORY, -3, "Memory error") \
    X(PRISMA_ERROR_VK, -4, "Vulkan error")     \
    X(PRISMA_ERROR_GLFW, -5, "GLFW error")     \
    X(PRISMA_ERROR_GLSL, -6, "Glslang error")

enum prisma_error
{
#define X(def, id, str) def=id,

    PRISMA_ERROR

#undef X
};

const char * prisma_error_str(enum prisma_error err);

#endif /* PRISMA_ERROR_H */