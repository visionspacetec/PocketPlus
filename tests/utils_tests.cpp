#include "pocketplusutils.h"
#include <gtest/gtest.h>
#include <string>
#include <stdio.h>

// void zero_stuffing(std::deque<bool>& in);
TEST(zero_stuffing, EmptyInput){
	std::deque<bool> in;
	pocketplus::utils::zero_stuffing(in);
	ASSERT_EQ(0, in.size());
}

TEST(zero_stuffing, StuffUntilByteBorder){
	std::deque<bool> in = {1};
	pocketplus::utils::zero_stuffing(in);
	ASSERT_EQ(8, in.size());
}

// void print_vector(const std::deque<bool>& in);
TEST(print_vector, AcceptEmptyInput){
	std::deque<bool> in;
	ASSERT_NO_THROW(pocketplus::utils::print_vector(in));
}

TEST(print_vector, PrintInput){
	std::deque<bool> in = {1, 0};
	std::stringstream buffer;
	std::streambuf *sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());
	pocketplus::utils::print_vector(in);
	std::cout.rdbuf(sbuf);
	ASSERT_EQ(std::string("1 0 \n"), buffer.str());
}

// std::deque<bool> number_to_deque_bool(long int& input, unsigned int& length);
TEST(number_to_deque_bool, AllZeroInput){
	auto in = std::make_unique<long>(0);
	auto length = std::make_unique<unsigned int>(0);
	ASSERT_EQ(std::deque<bool>(), pocketplus::utils::number_to_deque_bool(*in, *length));
}

TEST(number_to_deque_bool, ZeroInput){
	auto in = std::make_unique<long>(0);
	auto length = std::make_unique<unsigned int>(2);
	ASSERT_EQ(std::deque<bool>({0, 0}), pocketplus::utils::number_to_deque_bool(*in, *length));
}

TEST(number_to_deque_bool, ValidInput){
	auto in = std::make_unique<long>(1);
	auto length = std::make_unique<unsigned int>(8);
	ASSERT_EQ(std::deque<bool>({0, 0, 0, 0, 0, 0, 0, 1}), pocketplus::utils::number_to_deque_bool(*in, *length));
}

TEST(number_to_deque_bool, LargeOutput){
	auto in = std::make_unique<long>(1);
	auto length = std::make_unique<unsigned int>(33);
	ASSERT_EQ(std::deque<bool>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), pocketplus::utils::number_to_deque_bool(*in, *length));
}

//long int deque_bool_to_number(const std::deque<bool>& input);
TEST(deque_bool_to_number, EmptyInput){
	ASSERT_EQ(0, pocketplus::utils::deque_bool_to_number(std::deque<bool>()));
}

TEST(deque_bool_to_number, ZeroInput){
	ASSERT_EQ(0, pocketplus::utils::deque_bool_to_number(std::deque<bool>({0})));
}

TEST(deque_bool_to_number, ValidInput){
	ASSERT_EQ(3, pocketplus::utils::deque_bool_to_number(std::deque<bool>({0, 1, 1})));
}

// std::size_t divide_up(std::size_t dividend, std::size_t divisor);
TEST(divide_up, AllZeroInput){
	ASSERT_THROW(pocketplus::utils::divide_up(0, 0), std::invalid_argument);
}

TEST(divide_up, ZeroInput){
	ASSERT_EQ(0, pocketplus::utils::divide_up(0, 1));
}

TEST(divide_up, ValidInputRound){
	ASSERT_EQ(1, pocketplus::utils::divide_up(2, 3));
}

// std::string bool_to_string(std::deque<bool> const& boolvector);
TEST(bool_to_string, AcceptEmptyInput){
	std::deque<bool> in;
	ASSERT_EQ(std::string(), pocketplus::utils::bool_to_string(in));
}

TEST(bool_to_string, ValidInput){
	std::deque<bool> in = {1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_EQ(std::string("\xAA"), pocketplus::utils::bool_to_string(in));
}

// void write_bool_deque_to_file(const std::string& file_name, const std::deque<bool>& in);
TEST(write_bool_deque_to_file, ValidInput){
	std::deque<bool> in = {1, 0, 1, 0, 1, 0, 1, 0};
	ASSERT_NO_THROW(pocketplus::utils::write_bool_deque_to_file("test.bin", in));
	ASSERT_EQ(0, remove("test.bin"));
}

TEST(write_bool_deque_to_file, Symlink){
	std::deque<bool> in = {1, 0, 1, 0, 1, 0, 1, 0};
	std::filesystem::create_symlink("target", "test2.bin");
	ASSERT_THROW(pocketplus::utils::write_bool_deque_to_file("test2.bin", in), std::invalid_argument);
	std::filesystem::remove_all("test2.bin");
}

//std::deque<bool> read_bool_deque_from_file(const std::string& file_name);
// auto read_compressed = pocketplus::utils::read_bool_deque_from_file("compressed.bin");
TEST(read_bool_deque_from_file, ValidInput){
	std::deque<bool> in = {1, 0, 1, 0, 1, 0, 1, 0};
	pocketplus::utils::write_bool_deque_to_file("test.bin", in);
	auto read_test = pocketplus::utils::read_bool_deque_from_file("test.bin");
	ASSERT_EQ(in, read_test);
	remove("test.bin");
}

TEST(read_bool_deque_from_file, Symlink){
	std::filesystem::create_symlink("target", "test3.bin");
	ASSERT_THROW(pocketplus::utils::read_bool_deque_from_file("test3.bin"), std::invalid_argument);
	std::filesystem::remove_all("test3.bin");
}

// void pop_n_from_front(std::deque<bool>& in, const unsigned int& n);
TEST(pop_n_from_front, EmptyInput){
	std::deque<bool> in;
	ASSERT_THROW(pocketplus::utils::pop_n_from_front(in, 1), std::invalid_argument);
	ASSERT_EQ(std::deque<bool>(), in);
}

TEST(pop_n_from_front, ValidInput){
	std::deque<bool> in = {1, 1, 0};
	std::deque<bool> ref = {0};
	ASSERT_NO_THROW(pocketplus::utils::pop_n_from_front(in, 2));
	ASSERT_EQ(ref, in);
}