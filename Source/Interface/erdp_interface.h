#ifndef ERDP_INTERFACE_H
#define ERDP_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdint.h>
#include <stdio.h>
    typedef enum
    {
        ERDP_RESET = 0,
        ERDP_SET,
    } ERDP_Status_t;

#define erdp_assert(expr)                                   \
    do                                                      \
    {                                                       \
        if (!(expr))                                        \
        {                                                   \
            while (1)                                       \
                ; /* Infinite loop for assertion failure */ \
        }                                                   \
    } while (0)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ERDP_INTERFACE_H
