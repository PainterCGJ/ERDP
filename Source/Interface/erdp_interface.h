#ifndef ERDP_INTERFACE_H
#define ERDP_INTERFACE_H

typedef enum
{
    ERDP_REDSET = 0,
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

#endif // ERDP_INTERFACE_H
