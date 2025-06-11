#ifndef ERDP_IF_RTOS_H
#define ERDP_IF_RTOS_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "erdp_interface.h"

/* Configuration Constants */
#define DEFAULT_STACK_SIZE 128        /* Default task stack size in words */
#define OS_WAIT_FOREVER portMAX_DELAY /* Infinite blocking period */

/* Scheduler Control */
/**
 * @brief Starts the RTOS scheduler
 * @note This function will not return unless the scheduler is stopped
 */
void erdp_if_rtos_start_scheduler();

/**
 * @brief Converts milliseconds to RTOS ticks
 * @param[in] nms Time value in milliseconds
 * @return Time value converted to RTOS ticks
 */
uint32_t erdp_if_rtos_ms_to_ticks(uint32_t nms);

/* Queue API */
typedef QueueHandle_t OS_Queue;

/**
 * @brief Creates a new queue instance
 * @param[in] queue_length Maximum number of items the queue can hold
 * @param[in] item_size Size of each item in bytes
 * @return Handle to the created queue
 */
OS_Queue erdp_if_rtos_queue_create(uint32_t queue_length, uint32_t item_size);

/**
 * @brief Receives an item from a queue
 * @param[in] os_queue Handle to the queue
 * @param[out] pxdata Pointer to buffer that receives the item
 * @param[in] ticks_to_wait Maximum time to wait (in ticks)
 * @return true if successful, false if timeout or error
 */
bool erdp_if_rtos_queue_recv(OS_Queue os_queue, uint8_t *pxdata,
                             uint32_t ticks_to_wait);

/**
 * @brief Sends an item to a queue
 * @param[in] os_queue Handle to the target queue
 * @param[in] pxdata Pointer to the data to be sent
 * @param[in] ticks_to_wait Maximum time to wait (in ticks)
 * @return true if successful, false if timeout or error
 * @note The data is copied into the queue, not referenced
 */
bool erdp_if_rtos_queue_send(OS_Queue os_queue, uint8_t *pxdata,
                             uint32_t ticks_to_wait);

/**
 * @brief Overwrites an item in a queue (for length 1 queues)
 * @param[in] os_queue Handle to the target queue
 * @param[in] pxdata Pointer to the data to overwrite with
 * @return true if successful, false on error
 * @note Only works with queues that have length of 1
 */
bool erdp_if_rtos_queue_overwrite(OS_Queue os_queue, uint8_t *pxdata);

/**
 * @brief Deletes a queue and frees its memory
 * @param[in] os_queue Handle to the queue to delete
 * @note Ensure no tasks are blocked on this queue before deletion
 */
void erdp_if_rtos_queue_delet(OS_Queue os_queue);

/* List API */
typedef List_t *OS_List;
typedef ListItem_t OS_ListItem;

/**
 * @brief Creates a new linked list
 * @return Handle to the created list
 */
OS_List erdp_if_rtos_list_create(void);

/**
 * @brief Initializes a list item
 * @param[in,out] pxItem Pointer to the list item to initialize
 * @param[in] pxOwner Pointer to the item's owner object
 */
void erdp_if_rtos_list_item_init(OS_ListItem *pxItem, void *pxOwner);

/**
 * @brief Inserts an item at the end of a list
 * @param[in,out] list Handle to the target list
 * @param[in] pxItem Pointer to the list item to insert
 */
void erdp_if_rtos_list_insert_end(OS_List list, OS_ListItem *pxItem);

/**
 * @brief Removes an item from its containing list
 * @param[in,out] pxItem Pointer to the list item to remove
 * @return Number of items remaining in the list after removal
 */
uint32_t erdp_if_rtos_list_remove_item(OS_ListItem *pxItem);

/**
 * @brief Deletes a list item
 * @param[in] pxItem Pointer to the list item to delete
 * @param[in] free_owner Whether to free the owner memory
 */
void erdp_if_rtos_delet_list_item(OS_ListItem *pxItem, bool free_owner);

/**
 * @brief Deletes an entire list
 * @param[in] list Handle to the list to delete
 * @param[in] free_item Whether to free list items memory
 * @param[in] free_owner Whether to free owners' memory
 */
void erdp_if_rtos_delet_list(OS_List list, bool free_item, bool free_owner);

/**
 * @brief Prints list contents for debugging
 * @param[in] list Handle to the list to print
 */
void erdp_if_rtos_list_printf(OS_List list);

/* Event Group API */
typedef EventGroupHandle_t OS_Event;
typedef EventBits_t OS_EventBits;

/**
 * @brief Creates a new event group
 * @return Handle to the created event group
 */
OS_Event erdp_if_rtos_event_create(void);

/**
 * @brief Sets specified bits in an event group
 * @param[in,out] event Handle to the event group
 * @param[in] bits_to_set Event bits to set (bitwise OR)
 * @return The event group value before the bits were set
 * @note This is typically used to notify tasks waiting for these bits
 */
OS_EventBits erdp_if_rtos_set_event_bits(OS_Event event, OS_EventBits bits_to_set);

/**
 * @brief Clears specified bits in an event group
 * @param[in,out] event Handle to the event group
 * @param[in] bits_to_clear Event bits to clear (bitwise AND with complement)
 * @return The event group value before the bits were cleared
 */
OS_EventBits erdp_if_rtos_clear_event_bits(OS_Event event, OS_EventBits bits_to_clear);

/**
 * @brief Gets the current value of an event group
 * @param[in] event Handle to the event group
 * @return The current value of the event group bits
 */
OS_EventBits erdp_if_rtos_get_event_bits(OS_Event event);

/**
 * @brief Synchronizes with an event group (set and wait)
 * @param[in,out] event Handle to the event group
 * @param[in] bits_to_set Bits to set in the event group
 * @param[in] bits_wait_for Bits to wait for (bitwise OR)
 * @param[in] ticks_to_wait Maximum time to wait (in ticks)
 * @return The event group value at the time the bits were set
 * @note This atomically sets bits then waits for specified bits to be set
 */
OS_EventBits erdp_if_rtos_event_sync(OS_Event event, const OS_EventBits bits_to_set,
                                     const OS_EventBits bits_wait_for,
                                     uint32_t ticks_to_wait);

/* Semaphore API */
typedef enum
{
    BinarySemaphoreTag,        /* Binary semaphore type */
    MutexSemaphoreTag,         /* Mutex semaphore type */
    RecursiveMutexSemaphoreTag /* Recursive mutex type */
} Semaphore_tag;

typedef SemaphoreHandle_t OS_Semaphore;

/**
 * @brief Creates a semaphore of specified type
 * @param[in] tag Type of semaphore to create
 * @return Handle to the created semaphore
 */
OS_Semaphore erdp_if_rtos_semaphore_creat(Semaphore_tag tag);

/**
 * @brief Creates a counting semaphore
 * @param[in] max_count Maximum count value the semaphore can reach
 * @param[in] initial_count Initial count value of the semaphore
 * @return Handle to the created counting semaphore
 * @note The semaphore count decreases when taken and increases when given
 */
OS_Semaphore erdp_if_rtos_counting_semaphore_creat(uint32_t max_count, uint32_t initial_count);

/**
 * @brief Attempts to take (acquire) a semaphore
 * @param[in] semaphore Handle to the semaphore to take
 * @param[in] ticks_to_wait Maximum time to wait for the semaphore (in ticks)
 * @return true if semaphore was successfully taken, false if timeout occurred
 * @note For binary/mutex semaphores, this decrements the semaphore count
 */
bool erdp_if_rtos_semaphore_take(OS_Semaphore semaphore, uint32_t ticks_to_wait);

/**
 * @brief Releases (gives) a semaphore
 * @param[in] semaphore Handle to the semaphore to release
 * @return true if semaphore was successfully released, false on error
 * @note For binary/mutex semaphores, this increments the semaphore count
 */
bool erdp_if_rtos_semaphore_give(OS_Semaphore semaphore);

/**
 * @brief Attempts to take (acquire) a recursive mutex semaphore
 * @param[in] semaphore Handle to the recursive mutex semaphore
 * @param[in] ticks_to_wait Maximum time to wait (in ticks)
 * @return true if semaphore was successfully taken, false if timeout occurred
 * @note Can be called multiple times by the same task without blocking
 */
bool erdp_if_rtos_recursive_semaphore_take(OS_Semaphore semaphore, uint32_t ticks_to_wait);

/**
 * @brief Releases (gives) a recursive mutex semaphore
 * @param[in] semaphore Handle to the recursive mutex semaphore
 * @return true if semaphore was successfully released, false on error
 * @note Must be called the same number of times as the take operation
 */
bool erdp_if_rtos_recursive_semaphore_give(OS_Semaphore semaphore);

/* CPU Control */
/**
 * @brief Locks the CPU by disabling interrupts
 * @return Key value that must be used to unlock
 */
uint32_t erdp_if_rtos_cpu_lock(void);

/**
 * @brief Unlocks the CPU by restoring interrupts
 * @param[in] key Value returned from erdp_if_rtos_cpu_lock()
 */
void erdp_if_rtos_cpu_unlock(uint32_t key);

#endif
