#include "pocketplusdecompressor.h"
#include "pocketpluscompressor.h"
#include <gtest/gtest.h>

namespace pocketplus {
namespace decompressor {

TEST(PocketPlusDecompressor, LengthValid){
	ASSERT_NO_THROW(pocketplus::decompressor::PocketPlusDecompressor decompressor(128));
}

TEST(PocketPlusDecompressor, LengthTooSmall){
	ASSERT_THROW(pocketplus::decompressor::PocketPlusDecompressor decompressor(0), std::out_of_range);
}

TEST(PocketPlusDecompressor, LengthTooLarge){
	ASSERT_THROW(pocketplus::decompressor::PocketPlusDecompressor decompressor(65536), std::out_of_range);
}

TEST(hamming_weight, InputValid){
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(4, decompressor->hamming_weight(input_vector));
}

TEST(hamming_weight_in_range, InputValid){
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(4, decompressor->hamming_weight_in_range(input_vector.begin(), input_vector.end() - 1));
}

TEST(reverse, InputValid){
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	std::deque<bool> input_vector = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref = {0, 1, 0, 1, 0, 1, 0, 1};
	ASSERT_EQ(ref, decompressor->reverse(input_vector));
}

TEST(undo_rle, InputValidEmptyResult){
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	std::deque<bool> input_vector = {1, 0};
	std::deque<bool> output_vector;
	auto bit_position = input_vector.begin();
	std::deque<bool> ref = {};
	decompressor->undo_rle(input_vector, output_vector, bit_position);
	ASSERT_EQ(ref, output_vector);
}

TEST(get_input_vector_length, InputEmpty){
	std::deque<bool> input_vector = {};
	ASSERT_EQ(0, pocketplus::decompressor::PocketPlusDecompressor::get_input_vector_length(input_vector));
}

TEST(get_input_vector_length, FirstFrame){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(8, pocketplus::decompressor::PocketPlusDecompressor::get_input_vector_length(input_vector));
}

TEST(decompress, DecompressFirstDataFrame){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref = {1, 0, 1, 0, 1, 0, 1, 0};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector = decompressor->decompress(input_vector);
	ASSERT_EQ(ref, output_vector);
}

TEST(decompress, DecompressTwoIdenticalDataFrames){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1};
	std::deque<bool> ref = {1, 0, 1, 0, 1, 0, 1, 0};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref, output_vector_1);
	ASSERT_EQ(ref, output_vector_2);
}

TEST(decompress, DecompressTwoDataFramesChangeInOneLSB){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

TEST(decompress, DecompressTwoDataFramesChangeInOneMSB){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {0, 0, 1, 0, 1, 0, 1, 0};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

TEST(decompress, DecompressThreeDataFramesChangeInOneLSB){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	auto output_vector_3 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
	ASSERT_EQ(ref_1, output_vector_3);
}

TEST(decompress, DecompressTwoDataFramesNewMaskFlag){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

TEST(decompress, DecompressThreeDataFramesChangeInLSBNewMaskFlag){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> ref_3 = {1, 0, 1, 0, 1, 1, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	auto output_vector_3 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
	ASSERT_EQ(ref_3, output_vector_3);
}

TEST(decompress, DecompressThreeDataFramesChangeInLSBSendMaskFlag){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> ref_3 = {1, 0, 1, 0, 1, 1, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	auto output_vector_3 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
	ASSERT_EQ(ref_3, output_vector_3);
}

TEST(decompress, DecompressThreeDataFramesChangeInLSBUncompressedFlag){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> ref_3 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	auto output_vector_3 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
	ASSERT_EQ(ref_3, output_vector_3);
}

TEST(decompress, Decompress_e_t_zero){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

TEST(decompress, Decompress_c_t_zero){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 1};
	std::deque<bool> ref_3 = {1, 0, 1, 0, 1, 1, 1, 1};
	std::deque<bool> ref_4 = {1, 0, 1, 0, 1, 1, 1, 1};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	auto output_vector_3 = decompressor->decompress(input_vector);
	auto output_vector_4 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
	ASSERT_EQ(ref_3, output_vector_3);
	ASSERT_EQ(ref_4, output_vector_4);
}

TEST(decompress, Decompress_second_vector_zero){
	std::deque<bool> input_vector = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
	std::deque<bool> ref_1 = {1, 0, 1, 0, 1, 0, 1, 0};
	std::deque<bool> ref_2 = {1, 0, 1, 0, 1, 0, 1, 0};
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(8);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

TEST(decompress, DecompressTwoLongFramesChangeInOneMSB){
	std::deque<bool> ref_1;
	std::deque<bool> ref_2;
	ref_1.assign(64, 0);
	ref_2.assign(64, 0);
	ref_2.at(63) = 1;
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(64);
	std::deque<bool> input_vector;
	auto input_vector_1 = compressor->compress(ref_1, 0, 1, 1, 1);
	auto input_vector_2 = compressor->compress(ref_2, 0, 0, 0, 0);
	pocketplus::utils::zero_stuffing(input_vector_1);
	pocketplus::utils::zero_stuffing(input_vector_2);
	input_vector.insert(input_vector.end(), input_vector_1.begin(), input_vector_1.end());
	input_vector.insert(input_vector.end(), input_vector_2.begin(), input_vector_2.end());
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(64);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

TEST(decompress, DecompressTwoLongFramesChangeInMSBAndMiddle){
	std::deque<bool> ref_1;
	std::deque<bool> ref_2;
	ref_1.assign(64, 0);
	ref_2.assign(64, 0);
	ref_2.at(30) = 1;
	auto compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();
	compressor->set_input_vector_length(64);
	std::deque<bool> input_vector;
	auto input_vector_1 = compressor->compress(ref_1, 0, 1, 1, 1);
	auto input_vector_2 = compressor->compress(ref_2, 0, 0, 0, 0);
	pocketplus::utils::zero_stuffing(input_vector_1);
	pocketplus::utils::zero_stuffing(input_vector_2);
	input_vector.insert(input_vector.end(), input_vector_1.begin(), input_vector_1.end());
	input_vector.insert(input_vector.end(), input_vector_2.begin(), input_vector_2.end());
	auto decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(64);
	auto output_vector_1 = decompressor->decompress(input_vector);
	auto output_vector_2 = decompressor->decompress(input_vector);
	pocketplus::utils::print_vector(ref_1);
	pocketplus::utils::print_vector(ref_2);
	pocketplus::utils::print_vector(output_vector_1);
	pocketplus::utils::print_vector(output_vector_2);
	ASSERT_EQ(ref_1, output_vector_1);
	ASSERT_EQ(ref_2, output_vector_2);
}

};
};