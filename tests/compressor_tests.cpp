#include "pocketpluscompressor.h"
#include <gtest/gtest.h>

namespace pocketplus {
namespace compressor {

TEST(count, InputValid1){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_NO_THROW(compressor.count(1));
	std::deque<bool> ref = {0};
	ASSERT_EQ(ref, compressor.count(1));
}

TEST(count, InputTooSmall){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_THROW(compressor.count(0), std::out_of_range);
}

TEST(count, InputTooLarge){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_THROW(compressor.count(65536), std::out_of_range);
}

TEST(bit_extraction, InputValid){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	std::deque<bool> input1 = {1};
	std::deque<bool> input2 = {1};
	std::deque<bool> ref = {1};
	ASSERT_NO_THROW(compressor.bit_extraction(input1, input2));
	ASSERT_EQ(ref, compressor.bit_extraction(input1, input2));
}

TEST(bit_extraction, InputInvalid){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	std::deque<bool> input1 = {0, 0};
	std::deque<bool> input2 = {1};
	ASSERT_THROW(compressor.bit_extraction(input1, input2), std::invalid_argument);
}

TEST(set_input_vector_length, LengthValid){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_NO_THROW(compressor.set_input_vector_length(128));
}

TEST(set_input_vector_length, LengthTooSmall){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_THROW(compressor.set_input_vector_length(0), std::out_of_range);
}

TEST(set_input_vector_length, LengthTooLarge){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_THROW(compressor.set_input_vector_length(65536), std::out_of_range);
}

TEST(set_input_vector_length, AlreadySet){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_THROW(compressor.set_input_vector_length(8), std::invalid_argument);
}

TEST(get_input_vector_length, NotInitialized){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_EQ(0, compressor.get_input_vector_length());
}

TEST(get_input_vector_length, Initialized){
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_EQ(8, compressor.get_input_vector_length());
}

TEST(compress, InputVectorSizeNotInitialized){
	std::deque<bool> input_vector = {1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	ASSERT_THROW(compressor.compress(input_vector, 1, 1, 0, 1), std::invalid_argument);
}

TEST(compress, InputVectorSizeInvalid){
	std::deque<bool> input_vector = {1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_THROW(compressor.compress(input_vector, 1, 1, 0, 1), std::invalid_argument);
}

TEST(compress, RobustnessLevelTooLarge){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_THROW(compressor.compress(input_vector, 8, 1, 1, 1), std::out_of_range);
}

TEST(compress, SendMaskFlagNotSet){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_THROW(compressor.compress(input_vector, 0, 1, 0, 1), std::invalid_argument);
}

TEST(compress, UncompressedFlagNotSet){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_THROW(compressor.compress(input_vector, 0, 1, 1, 0), std::invalid_argument);
}

TEST(compress, PopMaskFlag){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	ASSERT_NO_THROW(compressor.compress(input_vector, 0, 1, 1, 1));
	for(unsigned int i; i < 16; i++){
		ASSERT_NO_THROW(compressor.compress(input_vector, 0, 0, 0, 0));
	}
}

TEST(compress, CompressFirstDataFrame){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	auto output_vector = compressor.compress(input_vector, 1, 1, 1, 1);
	std::deque<bool> ref = {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(ref, output_vector);
}

TEST(compress, CompressTwoIdenticalDataFrames){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	auto output_vector_1 = compressor.compress(input_vector, 0, 1, 1, 1);
	auto output_vector_2 = compressor.compress(input_vector, 0, 0, 0, 0);
	std::deque<bool> ref = {1, 0, 0, 0, 0, 1, 1};
	ASSERT_EQ(ref, output_vector_2);
}

TEST(compress, CompressThreeIdenticalDataFrames){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	auto output_vector_1 = compressor.compress(input_vector, 0, 1, 1, 1);
	auto output_vector_2 = compressor.compress(input_vector, 0, 0, 0, 0);
	auto output_vector_3 = compressor.compress(input_vector, 0, 0, 0, 0);
	std::deque<bool> ref = {1, 0, 0, 0, 1, 0, 1};
	ASSERT_EQ(ref, output_vector_3);
}

TEST(compress, CompressTwoDataFramesChangeInOneLSB){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	auto output_vector_1 = compressor.compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor.compress(input_vector_2, 0, 0, 0, 0);
	std::deque<bool> ref = {0, 1, 0, 0, 0, 0, 0, 1, 1};
	ASSERT_EQ(ref, output_vector_2);
}

TEST(compress, CompressThreeDataFramesChangeInOneLSB){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	auto output_vector_1 = compressor.compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor.compress(input_vector_2, 0, 0, 0, 0);
	auto output_vector_3 = compressor.compress(input_vector_1, 0, 0, 0, 0);
	std::deque<bool> ref = {1, 0, 0, 0, 0, 1, 1, 0};
	ASSERT_EQ(ref, output_vector_3);
}

TEST(compress, CompressTwoDataFramesNewMaskFlag){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = pocketplus::compressor::PocketPlusCompressor();
	compressor.set_input_vector_length(8);
	auto output_vector_1 = compressor.compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor.compress(input_vector_2, 0, 1, 0, 0);
	std::deque<bool> ref = {0, 1, 0, 0, 0, 0, 0, 1, 1};
	ASSERT_EQ(ref, output_vector_2);
}

};
};