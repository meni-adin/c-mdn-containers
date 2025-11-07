// NOLINTBEGIN(hicpp-vararg, hicpp-no-malloc)
#define MDN_LOGGER_SET_LEVEL_NONE

#include <array>
#include <cstdlib>

#include "mdn/gtest_extension.hpp"
#include "mdn/logger.h"
#include "mdn/mock_wrapper.hpp"
#include "vector.h"

using namespace testing;

#define MDN_VERIFY_SUCCESS(status, func, ...)                             \
    do {                                                                  \
        status = func(__VA_ARGS__);                                       \
        if (status != MDN_STATUS_SUCCESS) {                               \
            FAIL() << "Function " #func " failed with status " << status; \
        }                                                                 \
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

    // Helper: Insert sequential integers [0, 1, 2, ..., count-1] at the end
    static void InsertSequentialInts(mdn_Vector_t *vector, size_t count) {
        mdn_Status_t status;
        for (size_t i = 0; i < count; ++i) {
            int element = static_cast<int>(i);
            MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
        }
    }

    // Helper: Insert sequential integers starting from offset [offset, offset+1, ..., offset+count-1]
    static void InsertSequentialIntsWithOffset(mdn_Vector_t *vector, size_t count, int offset) {
        mdn_Status_t status;
        for (size_t i = 0; i < count; ++i) {
            int element = static_cast<int>(i) + offset;
            MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
        }
    }

    // Helper: Verify vector contains sequential integers [0, 1, 2, ..., count-1]
    static void VerifySequentialInts(mdn_Vector_t *vector, size_t count) {
        mdn_Status_t status;
        for (size_t i = 0; i < count; ++i) {
            int value = -1;
            MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
            ASSERT_EQ(value, static_cast<int>(i));
        }
    }

    // Helper: Verify vector contains sequential integers starting from offset
    static void VerifySequentialIntsWithOffset(mdn_Vector_t *vector, size_t count, int offset) {
        mdn_Status_t status;
        for (size_t i = 0; i < count; ++i) {
            int value = -1;
            MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
            ASSERT_EQ(value, static_cast<int>(i) + offset);
        }
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
#ifdef MDN_MW_ENABLE_MOCKING
class VectorMockingTest : public VectorTestBase {};
#endif  // MDN_MW_ENABLE_MOCKING

// ============================================================================
// Lifecycle Tests
// ============================================================================

TEST_F(VectorLifecycleTest, NewAndDelete) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    ASSERT_NE(vector, nullptr);

    mdn_Vector_delete(vector);
}

TEST_F(VectorRemoveTest, Remove) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    const size_t  elementCount = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), elementCount);
    for (size_t i = 0; i < elementCount; ++i) {
        int    element = static_cast<int>(i);
        size_t size    = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, i, &element);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, i + 1);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        int    value = -1;
        size_t size  = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 0, &value);
        ASSERT_EQ(value, static_cast<int>(i));
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, elementCount - i - 1);
    }
    mdn_Vector_delete(vector);
}

TEST_F(VectorRemoveTest, RemoveWithNullArgument) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    const size_t  elementCount = 5;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), elementCount);
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));

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
    mdn_Status_t  status;
    const size_t  elementCount = 50;
    constexpr int multiplier   = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));

    for (size_t i = 0; i < elementCount; ++i) {
        int value = static_cast<int>(i * multiplier);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_set, vector, i, &value);
    }
    for (size_t i = 0; i < elementCount; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, static_cast<int>(i * multiplier));
    }
    mdn_Vector_delete(vector);
}

TEST_F(VectorQueryTest, SizeIsEmptyAndClear) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    const size_t  elementCount = 20;
    size_t        size         = SIZE_MAX;
    bool          isEmpty      = false;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_isEmpty, vector, &isEmpty);
    ASSERT_TRUE(isEmpty);

    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));

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
    mdn_Status_t  status;
    size_t        capacity        = 0;
    const size_t  initialCapacity = 50;
    constexpr int numElements     = 30;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Add elements to fill the vector
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, numElements));

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
    mdn_Status_t  status;
    size_t        size          = 0;
    constexpr int initialCount  = 5;
    constexpr int newValueStart = 100;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Add initial elements
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, initialCount));

    // Clear the vector
    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);

    // Reuse the vector with new elements
    ASSERT_NO_FATAL_FAILURE(InsertSequentialIntsWithOffset(vector, initialCount, newValueStart));

    // Verify new elements
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, initialCount);
    ASSERT_NO_FATAL_FAILURE(VerifySequentialIntsWithOffset(vector, initialCount, newValueStart));

    mdn_Vector_delete(vector);
}

TEST_F(VectorQueryTest, ClearEmptyVector) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    size_t        size    = 0;
    bool          isEmpty = false;

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

TEST_F(VectorInsertTest, InsertAtEnd) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    const size_t  elementCount = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));
    ASSERT_NO_FATAL_FAILURE(VerifySequentialInts(vector, elementCount));
    mdn_Vector_delete(vector);
}

TEST_F(VectorInsertTest, InsertAtIndexEqualToSize) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    size_t        size       = 0;
    constexpr int count      = 5;
    constexpr int multiplier = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Insert at index == size is valid (appending)
    for (int i = 0; i < count; ++i) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, static_cast<size_t>(i));

        int value = i * multiplier;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, size, &value);  // Insert at index == size

        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, static_cast<size_t>(i + 1));
    }

    // Verify all elements
    for (int i = 0; i < count; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, static_cast<size_t>(i), &value);
        ASSERT_EQ(value, i * multiplier);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorInsertTest, InsertAtBeginning) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    const size_t  elementCount = 20;

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
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    constexpr int    initialCount = 5;
    constexpr int    middleValue  = 99;
    constexpr size_t expectedSize = 6;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Insert initial elements: [0, 1, 2, 3, 4]
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, initialCount));

    // Insert in middle at index 2: [0, 1, 99, 2, 3, 4]
    int middleVal = middleValue;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 2, &middleVal);

    // Verify sequence
    const std::array<int, expectedSize> expected = {0, 1, middleValue, 2, 3, 4};
    for (size_t i = 0; i < expectedSize; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, expected[i]);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ReservePreventingReallocation) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    size_t           capacity        = 0;
    constexpr size_t reserveCapacity = 100;
    constexpr int    elementCount    = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Reserve large capacity
    MDN_VERIFY_SUCCESS(status, mdn_Vector_reserve, vector, reserveCapacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, reserveCapacity);

    // Insert many elements - should not trigger reallocation
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));

    // Capacity should remain the same (no reallocation happened)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, reserveCapacity);

    // Verify elements are correct
    ASSERT_NO_FATAL_FAILURE(VerifySequentialInts(vector, elementCount));

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, MultipleReserveGrowthCycles) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    size_t           capacity          = 0;
    constexpr size_t initialCapacity   = 10;
    constexpr size_t startCapacity     = 20;
    constexpr size_t maxCapacity       = 100;
    constexpr size_t capacityIncrement = 20;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Multiple reserve operations with increasing capacity
    for (size_t targetCapacity = startCapacity; targetCapacity <= maxCapacity; targetCapacity += capacityIncrement) {
        MDN_VERIFY_SUCCESS(status, mdn_Vector_reserve, vector, targetCapacity);
        MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
        ASSERT_EQ(capacity, targetCapacity);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, CapacityAndReserve) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    size_t        capacity        = 0;
    const size_t  initialCapacity = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Verify initial capacity
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Reserve smaller capacity (should be no-op)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_reserve, vector, 5);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Reserve larger capacity
    const size_t newCapacity = 50;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_reserve, vector, newCapacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, newCapacity);

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, CapacityGrowth) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    const size_t  initialCapacity = 4;
    size_t        capacity        = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Insert enough elements to trigger growth (capacity should double)
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, initialCapacity + 1));

    // Capacity should have grown (at least doubled)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_GE(capacity, initialCapacity * 2);

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ShrinkToFit) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    size_t           capacity        = 0;
    size_t           size            = 0;
    constexpr size_t initialCapacity = 50;
    constexpr int    elementCount    = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Add only 10 elements
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    // Shrink to fit
    MDN_VERIFY_SUCCESS(status, mdn_Vector_shrinkToFit, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(capacity, size);
    ASSERT_EQ(capacity, elementCount);

    // Verify elements are still intact
    ASSERT_NO_FATAL_FAILURE(VerifySequentialInts(vector, elementCount));

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ShrinkToFitEmptyVector) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    size_t        capacity = 0;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 50);

    // Shrink empty vector (should result in capacity of 1)
    MDN_VERIFY_SUCCESS(status, mdn_Vector_shrinkToFit, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, 1);

    mdn_Vector_delete(vector);
}

TEST_F(VectorCapacityTest, ShrinkToFitWhenSizeEqualsCapacity) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    size_t           capacity       = 0;
    size_t           size           = 0;
    constexpr size_t vectorCapacity = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), vectorCapacity);

    // Fill vector to exact capacity
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, vectorCapacity));

    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(size, vectorCapacity);
    ASSERT_EQ(capacity, vectorCapacity);

    // Shrink to fit should be a no-op when size == capacity
    MDN_VERIFY_SUCCESS(status, mdn_Vector_shrinkToFit, vector);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, vectorCapacity);

    mdn_Vector_delete(vector);
}

// ============================================================================
// Remove Tests
// ============================================================================

TEST_F(VectorRemoveTest, RemoveFromMiddle) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    constexpr int    initialCount = 5;
    constexpr size_t expectedSize = 4;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Insert elements: [0, 1, 2, 3, 4]
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, initialCount));

    // Remove middle element at index 2 (value 2)
    int removed = -1;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 2, &removed);
    ASSERT_EQ(removed, 2);

    // Verify sequence is now [0, 1, 3, 4]
    const std::array<int, expectedSize> expected = {0, 1, 3, 4};
    for (size_t i = 0; i < expectedSize; ++i) {
        int value = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, i, &value);
        ASSERT_EQ(value, expected[i]);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorRemoveTest, RemoveFromBeginning) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int initialCount = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, initialCount));

    // Remove from beginning repeatedly
    for (int i = 0; i < initialCount; ++i) {
        int removed = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 0, &removed);
        ASSERT_EQ(removed, i);

        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, static_cast<size_t>(initialCount - i - 1));
    }

    // Vector should be empty
    size_t size = SIZE_MAX;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);

    mdn_Vector_delete(vector);
}

TEST_F(VectorRemoveTest, RemoveFromEnd) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int initialCount = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, initialCount));

    // Remove from end repeatedly
    for (int i = initialCount - 1; i >= 0; --i) {
        int removed = -1;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, static_cast<size_t>(i), &removed);
        ASSERT_EQ(removed, i);

        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, static_cast<size_t>(i));
    }

    // Vector should be empty
    size_t size = SIZE_MAX;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);

    mdn_Vector_delete(vector);
}

TEST_F(VectorEdgeCasesTest, InterleavedInsertAndRemove) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int iterations       = 20;
    constexpr int elementsToInsert = 3;
    constexpr int elementsToRemove = 2;
    constexpr int multiplier       = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 5);

    // Interleave inserts and removes
    for (int i = 0; i < iterations; ++i) {
        // Insert 3 elements
        for (int j = 0; j < elementsToInsert; ++j) {
            int value = (i * multiplier) + j;
            MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value);
        }

        // Remove 2 elements
        for (int j = 0; j < elementsToRemove; ++j) {
            int removed = -1;
            MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 0, &removed);
        }
    }

    // Should have net gain of 1 element per iteration
    size_t size = SIZE_MAX;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, iterations);

    mdn_Vector_delete(vector);
}

TEST_F(VectorEdgeCasesTest, ClearAndRefillMultipleTimes) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int cycles           = 5;
    constexpr int elementsPerCycle = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 5);

    for (int cycle = 0; cycle < cycles; ++cycle) {
        // Fill with elements
        ASSERT_NO_FATAL_FAILURE(InsertSequentialIntsWithOffset(vector, elementsPerCycle, cycle * 100));

        // Verify
        size_t size = SIZE_MAX;
        MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
        ASSERT_EQ(size, elementsPerCycle);

        ASSERT_NO_FATAL_FAILURE(VerifySequentialIntsWithOffset(vector, elementsPerCycle, cycle * 100));

        // Clear for next cycle
        MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);
    }

    mdn_Vector_delete(vector);
}

TEST_F(VectorEdgeCasesTest, SingleElementOperations) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int testValue = 42;
    size_t        size      = SIZE_MAX;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 1);

    // Insert single element
    int value = testValue;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 1);

    // Get single element
    int retrieved = -1;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, 0, &retrieved);
    ASSERT_EQ(retrieved, testValue);

    // Set single element
    int newValue = testValue * 2;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_set, vector, 0, &newValue);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, 0, &retrieved);
    ASSERT_EQ(retrieved, newValue);

    // Remove single element
    int removed = -1;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_remove, vector, 0, &removed);
    ASSERT_EQ(removed, newValue);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_size, vector, &size);
    ASSERT_EQ(size, 0);

    mdn_Vector_delete(vector);
}

TEST_F(VectorEdgeCasesTest, LargeElementSize) {
    constexpr size_t dataSize = 256;

    struct LargeStruct {
        std::array<char, dataSize> data;
        int                        id;
    };

    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int elementCount = 5;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(LargeStruct), 10);

    // Insert some large elements
    for (int i = 0; i < elementCount; ++i) {
        LargeStruct element{};
        element.id = i;
        static_cast<void>(snprintf(element.data.data(), element.data.size(), "Element %d", i));
        MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, static_cast<size_t>(i), &element);
    }

    // Verify elements
    for (int i = 0; i < elementCount; ++i) {
        LargeStruct value{};
        MDN_VERIFY_SUCCESS(status, mdn_Vector_get, vector, static_cast<size_t>(i), &value);
        ASSERT_EQ(value.id, i);
        std::array<char, dataSize> expected{};
        static_cast<void>(snprintf(expected.data(), expected.size(), "Element %d", i));
        ASSERT_STREQ(value.data.data(), expected.data());
    }

    mdn_Vector_delete(vector);
}

#ifdef MDN_CONTAINERS_SAFE_MODE
TEST_F(VectorSafeModeTest, NewInvalidArguments) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    constexpr size_t testCapacity = 10;

    // Null pointer
    status = mdn_Vector_new(nullptr, sizeof(int), testCapacity);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Zero element size
    status = mdn_Vector_new(&vector, 0, testCapacity);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Zero capacity
    status = mdn_Vector_new(&vector, sizeof(int), 0);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, GetInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    int           value;
    constexpr int testValue = 42;

    // Null vector
    status = mdn_Vector_get(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    int element = testValue;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &element);

    // Null element pointer
    status = mdn_Vector_get(vector, 0, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Clear vector for out of bounds tests
    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);

    // Out of bounds - empty vector
    status = mdn_Vector_get(vector, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Add one element and try to get beyond size
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &element);
    status = mdn_Vector_get(vector, 1, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, SetInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int testValue = 42;
    int           value     = testValue;

    // Null vector
    status = mdn_Vector_set(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value);

    // Null element pointer
    status = mdn_Vector_set(vector, 0, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Out of bounds - empty vector
    MDN_VERIFY_SUCCESS(status, mdn_Vector_clear, vector);
    status = mdn_Vector_set(vector, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Out of bounds - beyond size
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value);
    status = mdn_Vector_set(vector, 1, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, InsertInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    constexpr int testValue = 42;
    int           value     = testValue;

    // Null vector
    status = mdn_Vector_insert(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Null element pointer
    status = mdn_Vector_insert(vector, 0, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Index beyond size (inserting at index > size is invalid)
    status = mdn_Vector_insert(vector, 1, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Verify inserting at index == size is valid
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &value);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, RemoveInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    int           value;
    constexpr int testValue = 42;

    // Null vector
    status = mdn_Vector_remove(nullptr, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Out of bounds - empty vector
    status = mdn_Vector_remove(vector, 0, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Out of bounds - beyond size
    int element = testValue;
    MDN_VERIFY_SUCCESS(status, mdn_Vector_insert, vector, 0, &element);
    status = mdn_Vector_remove(vector, 1, &value);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    // Note: element pointer can be NULL (tested in RemoveWithNullArgument)

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, SizeInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    size_t        size;

    // Null vector
    status = mdn_Vector_size(nullptr, &size);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Null output pointer
    status = mdn_Vector_size(vector, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, CapacityInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    size_t        capacity;

    // Null vector
    status = mdn_Vector_capacity(nullptr, &capacity);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Null output pointer
    status = mdn_Vector_capacity(vector, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, IsEmptyInvalidArguments) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;
    bool          isEmpty;

    // Null vector
    status = mdn_Vector_isEmpty(nullptr, &isEmpty);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Null output pointer
    status = mdn_Vector_isEmpty(vector, nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, ClearInvalidArguments) {
    mdn_Status_t status;

    // Null vector
    status = mdn_Vector_clear(nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}

TEST_F(VectorSafeModeTest, ReserveInvalidArguments) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    constexpr size_t testCapacity = 100;

    // Null vector
    status = mdn_Vector_reserve(nullptr, testCapacity);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), 10);

    // Zero capacity
    status = mdn_Vector_reserve(vector, 0);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);

    mdn_Vector_delete(vector);
}

TEST_F(VectorSafeModeTest, ShrinkToFitInvalidArguments) {
    mdn_Status_t status;

    // Null vector
    status = mdn_Vector_shrinkToFit(nullptr);
    ASSERT_EQ(status, MDN_STATUS_ERROR_BAD_ARGUMENT);
}
#endif  // MDN_CONTAINERS_SAFE_MODE

#ifdef MDN_MW_ENABLE_MOCKING
// ============================================================================
// Memory Allocation Failure Tests
// ============================================================================

TEST_F(VectorMockingTest, NewMallocFailureOnVectorAllocation) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    constexpr size_t testCapacity = 10;

    // Mock only the first malloc to fail, let the second one use the fallback (real malloc)
    // This tests the case where vector struct allocation fails but data allocation would succeed
    EXPECT_CALL(*mWMock, malloc(StrEq("mdn_Vector_new"), _))
        .WillOnce(Return(nullptr))  // First malloc (vector struct) fails
        .RetiresOnSaturation();     // After first call, fall back to default behavior

    status = mdn_Vector_new(&vector, sizeof(int), testCapacity);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);
    ASSERT_EQ(vector, nullptr);
}

TEST_F(VectorMockingTest, NewMallocFailureOnDataAllocation) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    constexpr size_t testCapacity = 10;

    // Mock malloc to succeed for vector struct, fail for data array
    EXPECT_CALL(*mWMock, malloc(StrEq("mdn_Vector_new"), _))
        .WillOnce([](const char *, size_t size) { return std::malloc(size); })
        .WillOnce(Return(nullptr));

    status = mdn_Vector_new(&vector, sizeof(int), testCapacity);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);
    ASSERT_EQ(vector, nullptr);
}

TEST_F(VectorMockingTest, InsertReallocFailure) {
    mdn_Vector_t *vector = nullptr;
    mdn_Status_t  status;

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
    status     = mdn_Vector_insert(vector, 2, &value3);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);

    mdn_Vector_delete(vector);
}

TEST_F(VectorMockingTest, ReserveReallocFailure) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    size_t           capacity        = 0;
    constexpr size_t initialCapacity = 10;
    constexpr size_t newCapacity     = 50;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Mock realloc to fail when trying to reserve larger capacity
    EXPECT_CALL(*mWMock, realloc(StrEq("mdn_Vector_reserve"), _, _))
        .WillOnce(Return(nullptr));

    status = mdn_Vector_reserve(vector, newCapacity);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);

    // Verify capacity unchanged after failure
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    mdn_Vector_delete(vector);
}

TEST_F(VectorMockingTest, ShrinkToFitReallocFailure) {
    mdn_Vector_t    *vector = nullptr;
    mdn_Status_t     status;
    size_t           capacity        = 0;
    constexpr size_t initialCapacity = 50;
    constexpr int    elementCount    = 10;

    MDN_VERIFY_SUCCESS(status, mdn_Vector_new, &vector, sizeof(int), initialCapacity);

    // Add only 10 elements
    ASSERT_NO_FATAL_FAILURE(InsertSequentialInts(vector, elementCount));

    // Mock realloc to fail when trying to shrink
    EXPECT_CALL(*mWMock, realloc(StrEq("mdn_Vector_shrinkToFit"), _, _))
        .WillOnce(Return(nullptr));

    status = mdn_Vector_shrinkToFit(vector);

    ASSERT_EQ(status, MDN_STATUS_ERROR_MEM_ALLOC);

    // Verify capacity unchanged after failure
    MDN_VERIFY_SUCCESS(status, mdn_Vector_capacity, vector, &capacity);
    ASSERT_EQ(capacity, initialCapacity);

    mdn_Vector_delete(vector);
}
#endif  // MDN_MW_ENABLE_MOCKING

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// NOLINTEND(hicpp-vararg, hicpp-no-malloc)
