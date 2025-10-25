#include "mdn/gtest_extension.hpp"
#include "mdn/mock_wrapper.hpp"
#include "vector.h"

using namespace testing;

class VectorTest : public mdn::GTestExtension {
protected:
    void SetUp() override {
        mWMock = std::make_unique<MWMock>();
        MWMock::SetUp();
    }

    void TearDown() override {
        mWMock.reset(nullptr);
    }

public:
    static void SetUpTestSuite() {
        // Code to set up the test suite
    }
};

TEST_F(VectorTest, Hello) {
    mdn_Vector_hello();
}

#ifdef MDN_CONTAINER_SAFE_MODE
#endif  // MDN_CONTAINER_SAFE_MODE

#ifdef MDN_MW_ENABLE_MOCKING
#endif  // MDN_MW_ENABLE_MOCKING

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
