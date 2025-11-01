
#ifndef MDN_CONTAINER_H
#define MDN_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "mdn/status.h"

#define MDN_VECTOR_DEFAULT_CAPACITY 16

/**
 * @file vector.h
 * @brief Dynamic array (vector) implementation with optional safety checks.
 *
 * @note SAFETY MODE BEHAVIOR:
 * When MDN_CONTAINERS_SAFE_MODE is defined during compilation of vector.c, all functions perform comprehensive
 * parameter validation (NULL checks, bounds checks) and return appropriate error codes.
 *
 * When MDN_CONTAINERS_SAFE_MODE is NOT defined, parameter validation is skipped for
 * performance. Passing invalid parameters (NULL pointers, out-of-bounds indices) results
 * in undefined behavior. The caller is responsible for ensuring all parameters are valid.
 */

/**
 * @brief Opaque handle to a dynamic vector instance.
 *
 * This is a forward-declared structure that represents a dynamic array (vector).
 * The internal implementation details are hidden from the user. All operations
 * on the vector must be performed through the provided API functions.
 *
 * The vector stores elements of uniform size contiguously in memory and
 * automatically manages capacity growth as elements are added.
 */
typedef struct mdn_Vector_t_ mdn_Vector_t;

/**
 * Creates a new dynamic vector.
 *
 * @param[out] vector          Pointer to receive the newly created vector instance
 * @param[in]  elementSize     Size in bytes of each element to be stored
 * @param[in]  initialCapacity Initial capacity of the vector
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL, elementSize is 0, or initialCapacity is 0 (only in safe mode)
 * @return MDN_STATUS_ERROR_MEM_ALLOC if memory allocation fails
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_new(mdn_Vector_t **vector, size_t elementSize, size_t initialCapacity);

/**
 * Deletes a vector and frees all associated memory.
 *
 * @param[in] vector The vector to delete. If NULL, no operation is performed
 */
void mdn_Vector_delete(mdn_Vector_t *vector);

/**
 * Retrieves an element from the vector at the specified index.
 *
 * @param[in]  vector  The vector to get the element from
 * @param[in]  index   The index of the element to retrieve
 * @param[out] element Pointer to receive a copy of the element
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL, element is NULL, or index is out of bounds (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_get(mdn_Vector_t *vector, size_t index, void *element);

/**
 * Sets an element in the vector at the specified index.
 *
 * @param[in] vector  The vector to modify
 * @param[in] index   The index where the element should be set
 * @param[in] element Pointer to the element to copy into the vector
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL, element is NULL, or index is out of bounds (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_set(mdn_Vector_t *vector, size_t index, const void *element);

/**
 * Inserts an element into the vector at the specified index.
 * All elements at and after the specified index are shifted to the right.
 *
 * @param[in] vector  The vector to insert into
 * @param[in] index   The index where the element should be inserted (must be <= size)
 * @param[in] element Pointer to the element to insert
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL, element is NULL, or index > size (only in safe mode)
 * @return MDN_STATUS_ERROR_MEM_ALLOC if memory reallocation fails
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_insert(mdn_Vector_t *vector, size_t index, const void *element);

/**
 * Removes an element from the vector at the specified index.
 * All elements after the specified index are shifted to the left.
 *
 * @param[in]  vector  The vector to remove from
 * @param[in]  index   The index of the element to remove
 * @param[out] element Optional pointer to receive a copy of the removed element (can be NULL)
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL or index is out of bounds (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_remove(mdn_Vector_t *vector, size_t index, void *element);

/**
 * Gets the current number of elements in the vector.
 *
 * @param[in]  vector The vector to query
 * @param[out] size   Pointer to receive the current size
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL or size is NULL (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_size(const mdn_Vector_t *vector, size_t *size);

/**
 * Gets the current capacity of the vector.
 *
 * @param[in]  vector   The vector to query
 * @param[out] capacity Pointer to receive the current capacity
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL or capacity is NULL (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_capacity(const mdn_Vector_t *vector, size_t *capacity);

/**
 * Checks if the vector is empty.
 *
 * @param[in]  vector  The vector to check
 * @param[out] isEmpty Pointer to receive true if the vector is empty, false otherwise
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL or isEmpty is NULL (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_isEmpty(const mdn_Vector_t *vector, bool *isEmpty);

/**
 * Clears all elements from the vector without changing its capacity.
 *
 * @param[in] vector The vector to clear
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL (only in safe mode)
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_clear(mdn_Vector_t *vector);

/**
 * Reserves capacity for the vector. If the requested capacity is less than or equal
 * to the current capacity, no operation is performed.
 *
 * @param[in] vector   The vector to reserve capacity for
 * @param[in] capacity The desired minimum capacity
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL (only in safe mode)
 * @return MDN_STATUS_ERROR_MEM_ALLOC if memory reallocation fails
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_reserve(mdn_Vector_t *vector, size_t capacity);

/**
 * Shrinks the vector's capacity to match its current size.
 * If the vector is empty, capacity is set to 1.
 *
 * @param[in] vector The vector to shrink
 * @return MDN_STATUS_SUCCESS on success
 * @return MDN_STATUS_ERROR_BAD_ARGUMENT if vector is NULL (only in safe mode)
 * @return MDN_STATUS_ERROR_MEM_ALLOC if memory reallocation fails
 *
 * @warning Without MDN_CONTAINERS_SAFE_MODE, passing invalid parameters causes undefined behavior.
 */
mdn_Status_t mdn_Vector_shrinkToFit(mdn_Vector_t *vector);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // MDN_CONTAINER_H
