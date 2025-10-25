#define MDN_LOGGER_SET_LEVEL_DEBUG
#include "vector.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mdn/mock_wrapper.h"

struct mdn_Vector_t_ {
    void  *data;
    size_t size;
    size_t capacity;
    size_t elementSize;
};

#define MDN_VECTOR_INITIAL_CAPACITY 16
#define MDN_VECTOR_GROWTH_FACTOR    2

static void *mdn_Vector_getElementPtr(const mdn_Vector_t *vector, size_t index) {
    return (char *)vector->data + (index * vector->elementSize);
}

static mdn_Status_t mdn_Vector_ensureCapacity(mdn_Vector_t *vector, size_t requiredCapacity) {
    size_t newCapacity;
    void  *newData;

    if (vector->capacity >= requiredCapacity) {
        return MDN_STATUS_SUCCESS;
    }

    newCapacity = vector->capacity;
    while (newCapacity < requiredCapacity) {
        newCapacity *= MDN_VECTOR_GROWTH_FACTOR;
    }

    newData = realloc(vector->data, newCapacity * vector->elementSize);
    if (newData == NULL) {
        return MDN_STATUS_ERROR_MEM_ALLOC;
    }

    vector->data     = newData;
    vector->capacity = newCapacity;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_new(mdn_Vector_t **vector, size_t elementSize, size_t initialCapacity) {
    mdn_Status_t  status = MDN_STATUS_SUCCESS;
    mdn_Vector_t *vec    = NULL;
    void         *data   = NULL;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (elementSize == 0) || (initialCapacity == 0)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    vec  = malloc(sizeof(mdn_Vector_t));
    data = malloc(elementSize * initialCapacity);

    if ((vec == NULL) || (data == NULL)) {
        status = MDN_STATUS_ERROR_MEM_ALLOC;
        goto cleanup;
    }

    *vec = (mdn_Vector_t){
        .data        = data,
        .size        = 0,
        .capacity    = initialCapacity,
        .elementSize = elementSize};

    *vector = vec;
    return MDN_STATUS_SUCCESS;

cleanup:
    free(vec);
    free(data);
    return status;
}

void mdn_Vector_delete(mdn_Vector_t *vector) {
    if (vector != NULL) {
        free(vector->data);
        free(vector);
    }
}

mdn_Status_t mdn_Vector_get(mdn_Vector_t *vector, size_t index, void *element) {
    void *src;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (element == NULL) || (index >= vector->size)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    src = mdn_Vector_getElementPtr(vector, index);
    memcpy(element, src, vector->elementSize);

    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_set(mdn_Vector_t *vector, size_t index, const void *element) {
    void *dest;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (element == NULL) || (index >= vector->size)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

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
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    status = mdn_Vector_ensureCapacity(vector, vector->size + 1);
    if (status != MDN_STATUS_SUCCESS) {
        return status;
    }

    if (index < vector->size) {
        src         = mdn_Vector_getElementPtr(vector, index);
        dest        = mdn_Vector_getElementPtr(vector, index + 1);
        bytesToMove = (vector->size - index) * vector->elementSize;
        memmove(dest, src, bytesToMove);
    }

    insertDest = mdn_Vector_getElementPtr(vector, index);
    memcpy(insertDest, element, vector->elementSize);
    vector->size++;

    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_remove(mdn_Vector_t *vector, size_t index, void *element) {
    void  *src;
    void  *dest;
    size_t bytesToMove;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (index >= vector->size)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    if (element != NULL) {
        src = mdn_Vector_getElementPtr(vector, index);
        memcpy(element, src, vector->elementSize);
    }

    if (index < (vector->size - 1)) {
        dest        = mdn_Vector_getElementPtr(vector, index);
        src         = mdn_Vector_getElementPtr(vector, index + 1);
        bytesToMove = (vector->size - index - 1) * vector->elementSize;
        memmove(dest, src, bytesToMove);
    }

    vector->size--;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_size(const mdn_Vector_t *vector, size_t *size) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (size == NULL)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    *size = vector->size;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_capacity(const mdn_Vector_t *vector, size_t *capacity) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (capacity == NULL)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    *capacity = vector->capacity;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_isEmpty(const mdn_Vector_t *vector, bool *isEmpty) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (isEmpty == NULL)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    *isEmpty = (vector->size == 0);
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_clear(mdn_Vector_t *vector) {
#ifdef MDN_CONTAINERS_SAFE_MODE
    if (vector == NULL) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    vector->size = 0;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_reserve(mdn_Vector_t *vector, size_t capacity) {
    void *newData;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if ((vector == NULL) || (capacity == 0)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    if (capacity <= vector->capacity) {
        return MDN_STATUS_SUCCESS;
    }

    newData = realloc(vector->data, capacity * vector->elementSize);
    if (newData == NULL) {
        return MDN_STATUS_ERROR_MEM_ALLOC;
    }

    vector->data     = newData;
    vector->capacity = capacity;
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_Vector_shrinkToFit(mdn_Vector_t *vector) {
    void *newData;

#ifdef MDN_CONTAINERS_SAFE_MODE
    if (vector == NULL) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
#endif  // MDN_CONTAINERS_SAFE_MODE

    if (vector->size == vector->capacity) {
        return MDN_STATUS_SUCCESS;
    }

    if (vector->size == 0) {
        return mdn_Vector_reserve(vector, 1);
    }

    newData = realloc(vector->data, vector->size * vector->elementSize);
    if (newData == NULL) {
        return MDN_STATUS_ERROR_MEM_ALLOC;
    }

    vector->data     = newData;
    vector->capacity = vector->size;
    return MDN_STATUS_SUCCESS;
}
