#include "prisma/error.h"

const char* prisma_error_str(enum prisma_error err)
{
    switch (err)
    {
#define X(def,id,str)   \
    case id:            \
        return str;     \
    
    PRISMA_ERROR

#undef X

    default:
        return "Unknown error";
    }
}