#include "../src/utils/utils.h"
#include <gtest/gtest.h>

TEST(DivideUp, InsertZeros){
    ASSERT_THROW(pocketplus::utils::divide_up(0, 0), std::invalid_argument);
    //ASSERT_EQ(0, pocketplus::utils::divide_up(0, 0));
}

int main (int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}