#define MDN_LOGGER_SET_LEVEL_NONE

#include "mdn/gtest_extension.hpp"
#include "mdn/mock_wrapper.hpp"
#include "mdn/logger.h"
#include "vector.h"

using namespace testing;


#define MDN_VERIFY_SUCCESS(status, func, ...) \
    do { \
        status = func(__VA_ARGS__); \
        if (status != MDN_STATUS_SUCCESS) { \
            FAIL() << "Function " #func " failed with status " << status; \
        } \
    } while (0)

// Base test fixture with common setup/teardown
class VectorTestBase : public mdn::GTestExtension {
protected:
    void SetUp() override {
        mWMock = std::make_unique<MWMock>();
        MWMock::SetUp();
        mdn_Logger_init();
        mdn_Logger_addOutputStream(mdn_Logger_StreamConfig_t{stdout, MDN_LOGGER_LOGGING_LEVEL_DEBUG, MDN_LOGGER_LOGGING_FORMAT_SCREEN});
    }

    void TearDown() override {
        mdn_Logger_deinit();
        mWMock.reset(nullptr);
    }
};

// Test fixture classes
class VectorLifecycleTest : public VectorTestBase {};
class VectorInsertTest : public VectorTestBase {};
class VectorRemoveTest : public VectorTestBase {};
class VectorAccessTest : public VectorTestBase {};
class VectorQueryTest : public VectorTestBase {};
class VectorCapacityTest : public VectorTestBase {};
class VectorEdgeCasesTest : public VectorTestBase {};
#ifdef MDN_CONTAINERS_SAFE_MODE
class VectorSafeModeTest : public VectorTestBase {};
#endif  // MDN_CONTAINERS_SAFE_MODE

// ============================================================================
// Lifecycle Tests
// ============================================================================

TEST_F(VectorLifecycleTest, NewAndDelete) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    ASSERT_NE(vector, nullptr);

    mdn_Vector_delete(vector);
}

TEST_F(VectorInsertTest, InsertAtEnd) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t elementCount = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    for (size_t i = 0; i < elementCount; ++i) {
        int element = static_cast<int>(i);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, i);
    }
    mdn_Vector_delete(vector);
}

TEST_F(VectorRemoveTest, Remove) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t elementCount = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), elementCount);
    for (size_t i = 0; i < elementCount; ++i) {
        int element = static_cast<int>(i);
        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, i + 1);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        int value = -1;
        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 0, &value);
        ASSERT_EQ(value, static_cast<int>(i));
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, elementCount - i - 1);
    }
    mdn_Vector_delete(vector);
}

TEST_F(VectorRemoveTest, RemoveWithNullArgument) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t elementCount = 5;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), elementCount);
    for (size_t i = 0; i < elementCount; ++i) {
        int element = static_cast<int>(i);
        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, i + 1);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 0, nullptr);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, elementCount - i - 1);
    }
    mdn_Vector_delete(vector);
}

TEST_F(VectorAccessTest, SetAndGet) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t elementCount = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    for (size_t i = 0; i < elementCount; ++i) {
        int element = static_cast<int>(i);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        int value = static_cast<int>(i * 10);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_set, vector, i, &value);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, static_cast<int>(i * 10));
    }
    mdn_Vector_delete(vector);
}

TEST_F(VectorQueryTest, SizeIsEmptyAndClear) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t elementCount = 20;
    size_t size = SIZE_MAX;
    bool isEmpty = false;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_isEmpty, vector, &isEmpty);
    ASSERT_TRUE(isEmpty);

    for (size_t i = 0; i < elementCount; ++i) {
        int element = static_cast<int>(i);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
    }

    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, elementCount);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_isEmpty, vector, &isEmpty);
    ASSERT_FALSE(isEmpty);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_isEmpty, vector, &isEmpty);
    ASSERT_TRUE(isEmpty);

    mdn_Vector_delete(vector);
}

TEST_F(VectorQueryTest, ClearPreservesCapacity) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;
    size_t initialCapacity = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Add elements to fill the vector
    for (int i = 0; i < 30; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    // Get capacity before clear
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Clear the vector
    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);

    // Verify capacity remains unchanged
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    mdn_Vector_delete(vector);
}

TEST_F(VectorQueryTest, ClearAndReuse) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t size = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Add initial elements
    for (int i = 0; i < 5; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    // Clear the vector
    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);

    // Reuse the vector with new elements
    for (int i = 100; i < 105; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i - 100), &i);
    }

    // Verify new elements
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 5);

    for (int i = 0; i < 5; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, static_cast<size_t>(i), &value);
        ASSERT_EQ(value, i + 100);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorQueryTest, ClearEmptyVector) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t size = 0;
    bool isEmpty = false;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Clear already empty vector (should be no-op)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_isEmpty, vector, &isEmpty);
    ASSERT_TRUE(isEmpty);

    mdn_Vector_delete(vector);
}

// ============================================================================
// Insert Tests
// ============================================================================

TEST_F(VectorInsertTest, InsertAtBeginning) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t elementCount = 20;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Insert elements at the beginning (reverse order)
    for (size_t i = 0; i < elementCount; ++i) {
        int element = static_cast<int>(i);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &element);
    }

    // Verify elements are in reverse order
    for (size_t i = 0; i < elementCount; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, static_cast<int>(elementCount - i - 1));
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorInsertTest, InsertAtMiddle) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Insert initial elements: [0, 1, 2, 3, 4]
    for (int i = 0; i < 5; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    // Insert in middle at index 2: [0, 1, 99, 2, 3, 4]
    int middleValue = 99;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 2, &middleValue);

    // Verify sequence
    int expected[] = {0, 1, 99, 2, 3, 4};
    for (size_t i = 0; i < 6; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, expected[i]);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, CapacityAndReserve) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;
    size_t initialCapacity = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Verify initial capacity
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Reserve smaller capacity (should be no-op)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_reserve, vector, 5);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Reserve larger capacity
    size_t newCapacity = 50;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_reserve, vector, newCapacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, newCapacity);

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, CapacityGrowth) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t initialCapacity = 4;
    size_t capacity = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Insert enough elements to trigger growth (capacity should double)
    for (size_t i = 0; i < initialCapacity + 1; ++i) {
        int element = static_cast<int>(i);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
    }

    // Capacity should have grown (at least doubled)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_GE(capacity, initialCapacity * 2);

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ShrinkToFit) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;
    size_t size = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 50);

    // Add only 10 elements
    for (int i = 0; i < 10; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, 50);

    // Shrink to fit
    MDN_VERIFY_SUCCESS(status, mdn_Vector_shrinkToFit, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(capacity, size);
    ASSERT_EQ(capacity, 10);

    // Verify elements are still intact
    for (int i = 0; i < 10; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, static_cast<size_t>(i), &value);
        ASSERT_EQ(value, i);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ShrinkToFitEmptyVector) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 50);

    // Shrink empty vector (should result in capacity of 1)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_shrinkToFit, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, 1);

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ShrinkToFitWhenSizeEqualsCapacity) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;
    size_t size = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Fill vector to exact capacity
    for (int i = 0; i < 10; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(size, 10);
    ASSERT_EQ(capacity, 10);

    // Shrink to fit should be a no-op when size == capacity
    MDN_VERIFY_SUCCESS(status, mdn_Vector_shrinkToFit, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, 10);

    mdn_Vector_delete(vector);
}

// ============================================================================
// Remove Tests
// ============================================================================

TEST_F(VectorRemoveTest, RemoveFromMiddle) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Insert elements: [0, 1, 2, 3, 4]
    for (int i = 0; i < 5; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    // Remove middle element at index 2 (value 2)
    int removed = -1;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 2, &removed);
    ASSERT_EQ(removed, 2);

    // Verify sequence is now [0, 1, 3, 4]
    int expected[] = {0, 1, 3, 4};
    for (size_t i = 0; i < 4; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, expected[i]);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorEdgeCasesTest, LargeElementSize) {
    struct LargeStruct {
        char data[256];
        int id;
    };

    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(LargeStruct), 10);

    // Insert some large elements
    for (int i = 0; i < 5; ++i) {
        LargeStruct element;
        element.id = i;
        snprintf(element.data, sizeof(element.data), "Element %d", i);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &element);
    }

    // Verify elements
    for (int i = 0; i < 5; ++i) {
        LargeStruct value;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, static_cast<size_t>(i), &value);
        ASSERT_EQ(value.id, i);
        char expected[256];
        snprintf(expected, sizeof(expected), "Element %d", i);
        ASSERT_STREQ(value.data, expected);
    }

    mdn_Vector_delete(vector);
}

#ifdef MDN_CONTAINERS_SAFE_MODE
TEST_F(VectorSafeModeTest, NewWithNullPointer) {
    mdn_Status_t status;

    status = mdn_Vector_new(nullptr, sizeof(int), 10);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, NewWithZeroElementSize) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    status = mdn_Vector_new(&vector, 0, 10);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, NewWithZeroCapacity) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    status = mdn_Vector_new(&vector, sizeof(int), 0);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, GetWithNullVector) {
    mdn_Status_t status;
    int value;

    status = mdn_Vector_get(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, GetWithNullElement) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    int value = 42;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value);

    status = mdn_Vector_get(vector, 0, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, GetOutOfBounds) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    int value;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Try to get from empty vector
    status = mdn_Vector_get(vector, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Add one element
    int element = 42;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &element);

    // Try to get beyond size
    status = mdn_Vector_get(vector, 1, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, SetOutOfBounds) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    int value = 42;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Try to set in empty vector
    status = mdn_Vector_set(vector, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, InsertWithNullVector) {
    mdn_Status_t status;
    int value = 42;

    status = mdn_Vector_insert(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, InsertWithNullElement) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    status = mdn_Vector_insert(vector, 0, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, InsertBeyondSize) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    int value = 42;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Inserting at index > size is invalid
    status = mdn_Vector_insert(vector, 1, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, RemoveWithNullVector) {
    mdn_Status_t status;
    int value;

    status = mdn_Vector_remove(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, RemoveOutOfBounds) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    int value;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Try to remove from empty vector
    status = mdn_Vector_remove(vector, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, SizeWithNullVector) {
    mdn_Status_t status;
    size_t size;

    status = mdn_Vector_size(nullptr, &size);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, SizeWithNullOutput) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    status = mdn_Vector_size(vector, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, CapacityWithNullVector) {
    mdn_Status_t status;
    size_t capacity;

    status = mdn_Vector_capacity(nullptr, &capacity);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, IsEmptyWithNullVector) {
    mdn_Status_t status;
    bool isEmpty;

    status = mdn_Vector_isEmpty(nullptr, &isEmpty);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, ClearWithNullVector) {
    mdn_Status_t status;

    status = mdn_Vector_clear(nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, ReserveWithNullVector) {
    mdn_Status_t status;

    status = mdn_Vector_reserve(nullptr, 100);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, ReserveWithZeroCapacity) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    status = mdn_Vector_reserve(vector, 0);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, ShrinkToFitWithNullVector) {
    mdn_Status_t status;

    status = mdn_Vector_shrinkToFit(nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}
#endif  // MDN_CONTAINERS_SAFE_MODE

#ifdef MDN_MW_ENABLE_MOCKING
// ============================================================================
// Memory Allocation Failure Tests
// ============================================================================
class VectorMockingTest : public VectorTestBase {};

TEST_F(VectorMockingTest, NewMallocFailureOnVectorAllocation) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    // Mock only the first malloc to fail, let the second one use the fallback (real malloc)
    // This tests the case where vector struct allocation fails but data allocation would succeed
    EXPECT_CALL(*mWMock, malloc(StrEq("mdn_Vector_new"), _))
        .WillOnce(Return(nullptr))  // First malloc (vector struct) fails
        .RetiresOnSaturation();     // After first call, fall back to default behavior

    status = mdn_Vector_new(&vector, sizeof(int), 10);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);
    ASSERT_EQ(vector, nullptr);
}

TEST_F(VectorMockingTest, NewMallocFailureOnDataAllocation) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    // Mock malloc to succeed for vector struct, fail for data array
    EXPECT_CALL(*mWMock, malloc(StrEq("mdn_Vector_new"), _))
        .WillOnce([](const char*, size_t size) { return malloc(size); })
        .WillOnce(Return(nullptr));

    status = mdn_Vector_new(&vector, sizeof(int), 10);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);
    ASSERT_EQ(vector, nullptr);
}

TEST_F(VectorMockingTest, InsertReallocFailure) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 2);

    // Fill to capacity
    int value1 = 1;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value1);
    int value2 = 2;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 1, &value2);

    // Mock realloc to fail when trying to grow
    EXPECT_CALL(*mWMock, realloc(StrEq("mdn_Vector_ensureCapacity"), _, _))
        .WillOnce(Return(nullptr));

    int value3 = 3;
    status = mdn_Vector_insert(vector, 2, &value3);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);

    mdn_Vector_delete(vector);
}

TEST_F(VectorMockingTest, ReserveReallocFailure) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Mock realloc to fail when trying to reserve larger capacity
    EXPECT_CALL(*mWMock, realloc(StrEq("mdn_Vector_reserve"), _, _))
        .WillOnce(Return(nullptr));

    status = mdn_Vector_reserve(vector, 50);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);

    // Verify capacity unchanged after failure
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, 10);

    mdn_Vector_delete(vector);
}

TEST_F(VectorMockingTest, ShrinkToFitReallocFailure) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t status;
    size_t capacity = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 50);

    // Add only 10 elements
    for (int i = 0; i < 10; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &i);
    }

    // Mock realloc to fail when trying to shrink
    EXPECT_CALL(*mWMock, realloc(StrEq("mdn_Vector_shrinkToFit"), _, _))
        .WillOnce(Return(nullptr));

    status = mdn_Vector_shrinkToFit(vector);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);

    // Verify capacity unchanged after failure
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, 50);

    mdn_Vector_delete(vector);
}
#endif  // MDN_MW_ENABLE_MOCKING

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
