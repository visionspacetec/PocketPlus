#include "../src/compressor/compressor.h"
#include <gtest/gtest.h>

TEST(PocketPlusCompressor, LengthValid){
    auto input_vector_length = std::make_unique<unsigned int>(128);
    ASSERT_NO_THROW(pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length));
}

TEST(PocketPlusCompressor, LengthTooSmall){
    auto input_vector_length = std::make_unique<unsigned int>(7);
    ASSERT_THROW(pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length), std::out_of_range);
}

TEST(PocketPlusCompressor, LengthTooLarge){
    auto input_vector_length = std::make_unique<unsigned int>(65536);
    ASSERT_THROW(pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length), std::out_of_range);
}

TEST(compress, InputVectorSizeInvalid){
    std::deque<bool> input_vector = {1, 0, 1, 0};
    auto input_vector_length = std::make_unique<unsigned int>(8);
    pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length);
    ASSERT_THROW(compressor.compress(input_vector, 1, 1, 0, 1), std::invalid_argument);
}

TEST(compress, CompressFirstDataFrame){
    std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
    auto input_vector_length = std::make_unique<unsigned int>(8);
    pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length);
    auto output_vector = compressor.compress(input_vector, 1, 1, 0, 1);
    std::deque<bool> ref = {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0};
    ASSERT_EQ(ref, output_vector);
}