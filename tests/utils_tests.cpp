#include "../src/utils/utils.h"
#include <gtest/gtest.h>
#include <string>

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

/*
// Converts a long integer to a size n boolean deque
std::deque<bool> number_to_deque_bool(long int& input, std::unique_ptr<unsigned int>& length);

// Converts a boolean deque to long int
long int deque_bool_to_number(const std::deque<bool>& input);
*/

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

/*
// Converts a boolean vector to a string
std::string bool_to_string(std::deque<bool> const& boolvector);

// Saves a boolean deque to file
void write_bool_deque_to_file(const std::string& file_name, const std::deque<bool>& in);

// Reads as boolean deque from file
std::deque<bool> read_bool_deque_from_file(const std::string& file_name);

// Pops n elements from the front of a boolean deque
void pop_n_from_front(std::deque<bool>& in, unsigned int n);
*/