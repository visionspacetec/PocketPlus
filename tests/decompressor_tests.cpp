#include "../src/decompressor/decompressor.h"
#include <gtest/gtest.h>

TEST(PocketPlusDecompressor, LengthValid){
    auto input_vector_length = std::make_unique<unsigned int>(128);
    ASSERT_NO_THROW(pocketplus::decompressor::PocketPlusDecompressor decompressor(input_vector_length));
}

TEST(PocketPlusDecompressor, LengthTooSmall){
    auto input_vector_length = std::make_unique<unsigned int>(0);
    ASSERT_THROW(pocketplus::decompressor::PocketPlusDecompressor decompressor(input_vector_length), std::out_of_range);
}

TEST(PocketPlusDecompressor, LengthTooLarge){
    auto input_vector_length = std::make_unique<unsigned int>(65536);
    ASSERT_THROW(pocketplus::decompressor::PocketPlusDecompressor decompressor(input_vector_length), std::out_of_range);
}

TEST(decompress, DecompressFirstDataFrame){
    std::deque<bool> input_vector = {1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    std::deque<bool> ref = {1, 0, 1, 0, 1, 0, 1, 0};
    auto input_vector_length = std::make_unique<unsigned int>(8);
    pocketplus::decompressor::PocketPlusDecompressor decompressor(input_vector_length);
    auto output_vector = decompressor.decompress(input_vector);
    ASSERT_EQ(ref, output_vector);
}