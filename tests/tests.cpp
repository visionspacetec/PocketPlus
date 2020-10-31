#include "../src/utils/utils.h"
#include <gtest/gtest.h>

TEST(DivideUp, HandlesAllZeroInput){
    ASSERT_THROW(pocketplus::utils::divide_up(0, 0), std::invalid_argument);
}

TEST(DivideUp, HandlesZeroInput){
    ASSERT_EQ(0, pocketplus::utils::divide_up(0, 1));
}
