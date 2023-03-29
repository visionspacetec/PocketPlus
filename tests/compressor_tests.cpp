#include "pocketpluscompressor.h"
#include <gtest/gtest.h>

namespace pocketplus {
namespace compressor {

TEST(count, InputValid1){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_NO_THROW(compressor->count(1));
	std::deque<bool> ref = {0};
	auto output = compressor->count(1);
	ASSERT_EQ(ref, output);
}

TEST(count, InputTooSmall){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_THROW(compressor->count(0), std::out_of_range);
}

TEST(count, InputTooLarge){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_THROW(compressor->count(65536), std::out_of_range);
}

TEST(bit_extraction, InputValid){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	std::deque<bool> input1 = {1};
	std::deque<bool> input2 = {1};
	std::deque<bool> ref = {1};
	ASSERT_NO_THROW(compressor->bit_extraction(input1, input2));
	auto output = compressor->bit_extraction(input1, input2);
	ASSERT_EQ(ref, output);
}

TEST(bit_extraction, InputInvalid){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	std::deque<bool> input1 = {0, 0};
	std::deque<bool> input2 = {1};
	ASSERT_THROW(compressor->bit_extraction(input1, input2), std::invalid_argument);
}

TEST(set_input_vector_length, LengthValid){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_NO_THROW(compressor->set_input_vector_length(128));
}

TEST(set_input_vector_length, LengthTooSmall){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_THROW(compressor->set_input_vector_length(0), std::out_of_range);
}

TEST(set_input_vector_length, LengthTooLarge){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_THROW(compressor->set_input_vector_length(65536), std::out_of_range);
}

TEST(set_input_vector_length, AlreadySet){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_THROW(compressor->set_input_vector_length(8), std::invalid_argument);
}

TEST(get_input_vector_length, NotInitialized){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	auto output = compressor->get_input_vector_length();
	ASSERT_EQ(0, output);
}

TEST(get_input_vector_length, Initialized){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	auto output = compressor->get_input_vector_length();
	ASSERT_EQ(8, output);
}

TEST(set_initial_mask, LengthNotSet){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	std::deque<bool> mask = {1, 0, 1, 0};
	ASSERT_THROW(compressor->set_initial_mask(mask), std::invalid_argument);
}

TEST(set_initial_mask, LengthValid){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(4);
	std::deque<bool> mask = {1, 0, 1, 0};
	ASSERT_NO_THROW(compressor->set_initial_mask(mask));
}

TEST(set_initial_mask, LengthTooSmall){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(4);
	std::deque<bool> mask = {1, 0};
	ASSERT_THROW(compressor->set_initial_mask(mask), std::invalid_argument);
}

TEST(set_initial_mask, LengthTooLarge){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(4);
	std::deque<bool> mask = {1, 0, 1, 0, 1, 0};
	ASSERT_THROW(compressor->set_initial_mask(mask), std::invalid_argument);
}

TEST(compress, InputVectorSizeNotInitialized){
	std::deque<bool> input_vector = {1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	ASSERT_THROW(compressor->compress(input_vector, 1, 1, 0, 1), std::invalid_argument);
}

TEST(compress, InputVectorSizeInvalid){
	std::deque<bool> input_vector = {1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_THROW(compressor->compress(input_vector, 1, 1, 0, 1), std::invalid_argument);
}

TEST(compress, RobustnessLevelTooLarge){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_THROW(compressor->compress(input_vector, 8, 1, 1, 1), std::out_of_range);
}

TEST(compress, SendMaskFlagNotSet){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_THROW(compressor->compress(input_vector, 0, 1, 0, 1), std::invalid_argument);
}

TEST(compress, UncompressedFlagNotSet){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_THROW(compressor->compress(input_vector, 0, 1, 1, 0), std::invalid_argument);
}

TEST(compress, PopMaskFlag){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_NO_THROW(compressor->compress(input_vector, 0, 1, 1, 1));
	for(unsigned int i = 0; i < 16; i++){
		ASSERT_NO_THROW(compressor->compress(input_vector, 0, 0, 0, 0));
	}
}

TEST(compress, CompressFirstDataFrame){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	auto output_vector = compressor->compress(input_vector, 1, 1, 1, 1);
	std::deque<bool> ref = {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(ref, output_vector);
}

TEST(compress, CompressTwoIdenticalDataFrames){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector, 0, 0, 0, 0);
	std::deque<bool> ref = {1, 0, 0, 0, 0, 1, 1};
	ASSERT_EQ(ref, output_vector_2);
}

TEST(compress, CompressFourIdenticalDataFrames){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector, 0, 1, 1, 1);
	compressor->compress(input_vector, 0, 0, 0, 0);
	auto output_vector_3 = compressor->compress(input_vector, 0, 0, 0, 0);
	auto output_vector_4 = compressor->compress(input_vector, 0, 0, 0, 0);
	std::deque<bool> ref3 = {1, 0, 0, 0, 1, 0, 1}; // V_t = 2
	ASSERT_EQ(ref3, output_vector_3);
	std::deque<bool> ref4 = {1, 0, 0, 0, 1, 1, 1}; // V_t = 3
	ASSERT_EQ(ref4, output_vector_4);
}

TEST(compress, CompressTwoDataFramesChangeInOneLSB){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 0, 0, 0, 0);
	std::deque<bool> ref = {0, 1, 0, 0, 0, 0, 1, 0, 1, 1};
	ASSERT_EQ(ref, output_vector_2);
}

TEST(compress, CompressTwoDataFramesChangeInOneMSB){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {0, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 0, 0, 0, 0);
	std::deque<bool> ref = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	ASSERT_EQ(ref, output_vector_2);
}

TEST(compress, CompressThreeDataFramesChangeInOneLSB){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	compressor->compress(input_vector_2, 0, 0, 0, 0);
	auto output_vector_3 = compressor->compress(input_vector_1, 0, 0, 0, 0);
	std::deque<bool> ref = { 1, 0, 0, 0, 0, 0, 1, 0 };
	ASSERT_EQ(ref, output_vector_3);
}

TEST(compress, CompressTwoDataFramesNewMaskFlag){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 0, 1, 0, 0);
	std::deque<bool> ref = { 0, 1, 0, 0, 0, 0, 1, 0, 1, 1 };
	ASSERT_EQ(ref, output_vector_2);
}

TEST(compress, CompressThreeDataFramesChangeInLSBNewMaskFlagR1){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> input_vector_3 = {1, 0, 1, 0, 1, 1, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 0, 1, 0, 0);
	auto output_vector_3 = compressor->compress(input_vector_3, 1, 1, 0, 0);
	std::deque<bool> ref2 = { 0, 1, 0, 0, 0, 0, 1, 0, 1, 1 };
	std::deque<bool> ref3 = { 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1 };
	ASSERT_EQ(ref2, output_vector_2);
	ASSERT_EQ(ref3, output_vector_3);
}

TEST(compress, CompressThreeDataFramesChangeInLSBSendMaskFlagR1){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> input_vector_3 = {1, 0, 1, 0, 1, 1, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 0, 1, 0, 0);
	auto output_vector_3 = compressor->compress(input_vector_3, 1, 1, 1, 0);
	std::deque<bool> ref2 = { 0, 1, 0, 0, 0, 0, 1, 0, 1, 1 };
	std::deque<bool> ref3 = {0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1};
	ASSERT_EQ(ref2, output_vector_2);
    for(auto item : output_vector_3)
        printf("%i, ",item);
	ASSERT_EQ(ref3, output_vector_3);
}

TEST(compress, CompressThreeDataFramesChangeInLSBSendMaskFlag){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> input_vector_3 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 0, 0, 0, 0);
	auto output_vector_3 = compressor->compress(input_vector_3, 0, 0, 1, 0);
	std::deque<bool> ref2 = { 0, 1, 0, 0, 0, 0, 1, 0, 1, 1 };
	std::deque<bool> ref3 = { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1 };
	ASSERT_EQ(ref2, output_vector_2);
	ASSERT_EQ(ref3, output_vector_3);
}

TEST(compress, e_t_zero){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 1, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector_2, 1, 0, 1, 1);
	std::deque<bool> ref2 = {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1};
	ASSERT_EQ(ref2, output_vector_2);
}

TEST(compress, c_t_zero){
	std::deque<bool> input_vector_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> input_vector_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> input_vector_3 = {1, 0, 1, 0, 1, 1, 1, 1};
	std::deque<bool> input_vector_4 = {1, 0, 1, 0, 1, 1, 1, 1};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector_1, 0, 1, 1, 1);
	compressor->compress(input_vector_2, 0, 1, 0, 0);
	compressor->compress(input_vector_3, 1, 1, 1, 0);
	auto output_vector_4 = compressor->compress(input_vector_4, 1, 0, 0, 0);
	std::deque<bool> ref4 = { 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1 };
	ASSERT_EQ(ref4, output_vector_4);
}

TEST(compress, second_vector_zero){
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	compressor->compress(input_vector, 0, 1, 1, 1);
	auto output_vector_2 = compressor->compress(input_vector, 0, 0, 0, 1);
	std::deque<bool> ref2 = {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(ref2, output_vector_2);
}

TEST(constructor, copy_constructor){
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(8);
	ASSERT_NO_THROW(auto copied_compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>(*compressor));
}

};
};
