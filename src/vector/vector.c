#include "vector.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mdn/logger.h"
#include "mdn/mock_wrapper.h"

struct mdn_Vector_t_ {
    void  *data;
    size_t size;
    size_t capacity;
    size_t elementSize;
};

#define MDN_VECTOR_GROWTH_FACTOR    2

static void *mdn_Vector_getElementPtr(const mdn_Vector_t *vector, size_t index) {
    void *ptr;

    ptr = (char *)vector->data + (index * vector->elementSize);
    MDN_LOGGER_LOG_DEBUG("Getting element pointer: vector=%p, index=%zu, ptr=%p", (void *)vector, index, ptr);
    return ptr;
}

static mdn_Status_t mdn_Vector_ensureCapacity(mdn_Vector_t *vector, size_t requiredCapacity) {
    size_t newCapacity;
    void  *newData;
    size_t newSizeInBytes;

    MDN_LOGGER_LOG_DEBUG("Ensuring capacity: vector=%p, current=%zu, required=%zu",
                         (void *)vector, vector->capacity, requiredCapacity);

    if (vector->capacity >= requiredCapacity) {
        MDN_LOGGER_LOG_DEBUG("Capacity sufficient, no reallocation needed");
        return MDN_STATUS_SUCCESS;
    }

    newCapacity = vector->capacity;
    while (newCapacity < requiredCapacity) {
        newCapacity *= MDN_VECTOR_GROWTH_FACTOR;
    }

    MDN_LOGGER_LOG_DEBUG("Reallocating: oldCapacity=%zu, newCapacity=%zu", vector->capacity, newCapacity);

    newSizeInBytes = newCapacity * vector->elementSize;
    newData        = MDN_MW_realloc(vector->data, newSizeInBytes);
    if (newData == NULL) {
        MDN_LOGGER_LOG_ERROR("Reallocation failed: requested size=%zu bytes", newSizeInBytes);
        return MDN_STATUS_ERROR_MEM_ALLOC;
    }

    vector->data     = newData;
    vector->capacity = newCapacity;
    MDN_LOGGER_LOG_DEBUG("Reallocation successful: newData=%p, newCapacity=%zu", newData, newCapacity);
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_new(mdn_Vector_t **vector, size_t elementSize, size_t initialCapacity) {
    mdn_Status_t  status = MDN_STATUS_SUCCESS;
    mdn_Vector_t *vec    = NULL;
    void         *data   = NULL;

    MDN_LOGGER_LOG_DEBUG("Creating new vector: elementSize=%zu, initialCapacity=%zu", elementSize, initialCapacity);

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (elementSize == 0) || (initialCapacity == 0)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, elementSize=%zu, initialCapacity=%zu",
                             (void *)vector, elementSize, initialCapacity);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    vec  = MDN_MW_malloc(sizeof(mdn_Vector_t));
    data = MDN_MW_malloc(elementSize * initialCapacity);

    if ((vec == NULL) || (data == NULL)) {
        MDN_LOGGER_LOG_ERROR("Memory allocation failed: vec=%p, data=%p", (void *)vec, (void *)data);
        status = MDN_STATUS_ERROR_MEM_ALLOC;
        goto cleanup;
    }

    *vec = (mdn_Vector_t){
        .data        = data,
        .size        = 0,
        .capacity    = initialCapacity,
        .elementSize = elementSize};

    *vector = vec;
    MDN_LOGGER_LOG_DEBUG("Vector created successfully at %p", (void *)*vector);
    return MDN_STATUS_SUCCESS;

cleanup:
    free(vec);
    free(data);
    MDN_LOGGER_LOG_DEBUG("Cleanup performed, returning error status");
    return status;
}

void mdn_Vector_delete(mdn_Vector_t *vector) {
    if (vector != NULL) {
        MDN_LOGGER_LOG_INFO("Deleting vector at %p", (void *)vector);
        free(vector->data);
        free(vector);
    } else {
        MDN_LOGGER_LOG_DEBUG("Delete called with NULL vector, no action taken");
    }
}

mdn_Status_t mdn_Vector_get(mdn_Vector_t *vector, size_t index, void *element) {
    void *src;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (element == NULL) || (index >= vector->size)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, element=%p, index=%zu, size=%zu",
                             (void *)vector, element, index, vector ? vector->size : 0);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    MDN_LOGGER_LOG_DEBUG("Getting element: vector=%p, index=%zu, elementSize=%zu",
                         (void *)vector, index, vector->elementSize);

    src = mdn_Vector_getElementPtr(vector, index);
    memcpy(element, src, vector->elementSize);

    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_set(mdn_Vector_t *vector, size_t index, const void *element) {
    void *dest;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (element == NULL) || (index >= vector->size)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, element=%p, index=%zu, size=%zu",
                             (void *)vector, element, index, vector ? vector->size : 0);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    MDN_LOGGER_LOG_DEBUG("Setting element: vector=%p, index=%zu, elementSize=%zu",
                         (void *)vector, index, vector->elementSize);

    dest = mdn_Vector_getElementPtr(vector, index);
    memcpy(dest, element, vector->elementSize);

    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_insert(mdn_Vector_t *vector, size_t index, const void *element) {
    mdn_Status_t status;
    void        *src;
    void        *dest;
    size_t       bytesToMove;
    void        *insertDest;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (element == NULL) || (index > vector->size)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, element=%p, index=%zu, size=%zu",
                             (void *)vector, element, index, vector ? vector->size : 0);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    MDN_LOGGER_LOG_INFO("Inserting element: vector=%p, index=%zu, size=%zu, capacity=%zu",
                        (void *)vector, index, vector->size, vector->capacity);

    status = mdn_Vector_ensureCapacity(vector, vector->size + 1);
    if (status != MDN_STATUS_SUCCESS) {
        MDN_LOGGER_LOG_ERROR("Failed to ensure capacity for insert");
        return status;
    }

    if (index < vector->size) {
        src         = mdn_Vector_getElementPtr(vector, index);
        dest        = mdn_Vector_getElementPtr(vector, index + 1);
        bytesToMove = (vector->size - index) * vector->elementSize;
        MDN_LOGGER_LOG_DEBUG("Shifting elements: from index %zu, bytes=%zu", index, bytesToMove);
        memmove(dest, src, bytesToMove);
    }

    insertDest = mdn_Vector_getElementPtr(vector, index);
    memcpy(insertDest, element, vector->elementSize);
    ++(vector->size);

    MDN_LOGGER_LOG_DEBUG("Insert successful: new size=%zu", vector->size);
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_remove(mdn_Vector_t *vector, size_t index, void *element) {
    void  *src;
    void  *dest;
    size_t bytesToMove;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (index >= vector->size)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, index=%zu, size=%zu",
                             (void *)vector, index, vector ? vector->size : 0);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    MDN_LOGGER_LOG_INFO("Removing element: vector=%p, index=%zu, size=%zu, outputElement=%s",
                        (void *)vector, index, vector->size, element != NULL ? "yes" : "no");

    if (element != NULL) {
        src = mdn_Vector_getElementPtr(vector, index);
        memcpy(element, src, vector->elementSize);
    }

    if (index < (vector->size - 1)) {
        dest        = mdn_Vector_getElementPtr(vector, index);
        src         = mdn_Vector_getElementPtr(vector, index + 1);
        bytesToMove = (vector->size - index - 1) * vector->elementSize;
        MDN_LOGGER_LOG_DEBUG("Shifting elements: from index %zu, bytes=%zu", index + 1, bytesToMove);
        memmove(dest, src, bytesToMove);
    }

    --(vector->size);
    MDN_LOGGER_LOG_DEBUG("Remove successful: new size=%zu", vector->size);
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_size(const mdn_Vector_t *vector, size_t *size) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (size == NULL)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, size=%p", (void *)vector, (void *)size);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    *size = vector->size;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_capacity(const mdn_Vector_t *vector, size_t *capacity) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (capacity == NULL)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, capacity=%p", (void *)vector, (void *)capacity);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    *capacity = vector->capacity;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_isEmpty(const mdn_Vector_t *vector, bool *isEmpty) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (isEmpty == NULL)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, isEmpty=%p", (void *)vector, (void *)isEmpty);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    *isEmpty = (vector->size == 0);
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_clear(mdn_Vector_t *vector) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if (vector == NULL) {
        MDN_LOGGER_LOG_ERROR("Invalid argument: vector is NULL");
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    MDN_LOGGER_LOG_INFO("Clearing vector: vector=%p, old size=%zu", (void *)vector, vector->size);
    vector->size = 0;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_reserve(mdn_Vector_t *vector, size_t capacity) {
    void *newData;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (capacity == 0)) {
        MDN_LOGGER_LOG_ERROR("Invalid arguments: vector=%p, capacity=%zu", (void *)vector, capacity);
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    if (capacity <= vector->capacity) {
        MDN_LOGGER_LOG_DEBUG("Reserve not needed: requested=%zu, current=%zu", capacity, vector->capacity);
        return MDN_STATUS_SUCCESS;
    }

    MDN_LOGGER_LOG_INFO("Reserving capacity: vector=%p, old=%zu, new=%zu",
                        (void *)vector, vector->capacity, capacity);

    newData = MDN_MW_realloc(vector->data, capacity * vector->elementSize);
    if (newData == NULL) {
        MDN_LOGGER_LOG_ERROR("Reallocation failed: requested capacity=%zu, size=%zu bytes",
                             capacity, capacity * vector->elementSize);
        return MDN_STATUS_ERROR_MEM_ALLOC;
    }

    vector->data     = newData;
    vector->capacity = capacity;
    MDN_LOGGER_LOG_DEBUG("Reserve successful: newData=%p", newData);
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_shrinkToFit(mdn_Vector_t *vector) {
    void *newData;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if (vector == NULL) {
        MDN_LOGGER_LOG_ERROR("Invalid argument: vector is NULL");
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    size_t targetCapacity;
    size_t newSizeInBytes;

    if (vector->size == vector->capacity) {
        MDN_LOGGER_LOG_DEBUG("Shrink not needed: size=%zu equals capacity", vector->size);
        return MDN_STATUS_SUCCESS;
    }

    targetCapacity = vector->size > 0 ? vector->size : 1;
    newSizeInBytes = targetCapacity * vector->elementSize;

    MDN_LOGGER_LOG_INFO("Shrinking to fit: vector=%p, old capacity=%zu, new capacity=%zu",
                        (void *)vector, vector->capacity, targetCapacity);

    newData = MDN_MW_realloc(vector->data, newSizeInBytes);
    if (newData == NULL) {
        MDN_LOGGER_LOG_ERROR("Reallocation failed: requested size=%zu bytes", newSizeInBytes);
        return MDN_STATUS_ERROR_MEM_ALLOC;
    }

    vector->data     = newData;
    vector->capacity = targetCapacity;
    MDN_LOGGER_LOG_DEBUG("Shrink successful: newData=%p, capacity=%zu", newData, targetCapacity);
    return MDN_STATUS_SUCCESS;
}
