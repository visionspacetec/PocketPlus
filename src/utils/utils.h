#ifndef UTILS_H
#define UTILS_H

#include <deque>
#include <iostream>
#include <memory>
#include <cstddef>
#include <fstream>
#include <iterator>

namespace pocketplus {
namespace utils {

// Helper function for file saving
void zero_stuffing(std::deque<bool>& in);

// Prints a deque
void print_vector(const std::deque<bool>& in);

// Converts a long integer to a size n boolean vector
std::deque<bool> number_to_deque_bool(std::unique_ptr<long int>& input, std::unique_ptr<unsigned int>& length);

// Helper function for bool_to_string
std::size_t divide_up(std::size_t dividend, std::size_t divisor);

// Converts a boolean vector to a string
std::string bool_to_string(std::deque<bool> const& boolvector);

// Saves a boolean deque to file
void write_bool_deque_to_file(const std::string& file_name, const std::deque<bool>& in);

// Reads as boolean deque from file
std::deque<bool> read_bool_deque_from_file(const std::string& file_name);

}
}

#endif