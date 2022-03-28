#ifndef UTILS_H
#define UTILS_H

#include <deque>
#include <iostream>
#include <memory>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <filesystem>

namespace pocketplus {
namespace utils {

//! Helper function for file saving
/*!
	When saving to file it is necessary to have a eight bit (one byte) aligned boolean deque
	\param in Reference to the boolean deque to attach zeros to
*/
void zero_stuffing(std::deque<bool>& in);

//! Prints a boolean deque
/*!
	This function helps when debugging or to show subvectors on the terminal while processing
	\param in Boolean deque to print
*/
void print_vector(const std::deque<bool>& in);

//! Converts a long integer to a size n boolean deque
/*!
	When testing with long integers this converts them to processable boolen deques
	\param input Long integer to convert
	\param length The wanted size of the resulting boolean deque
	\return The long integer of requested length represented as boolean deque
*/
std::deque<bool> number_to_deque_bool(const long int& input, const unsigned int& length);

//! Converts a boolean deque back to the long integer format
/*!
	When testing with long integers this converts the boolean deques back to long integers after decompression
	\param input Boolen deque to convert back
	\return The back converted long integer
*/
long int deque_bool_to_number(const std::deque<bool>& input);

//! Helper function for bool_to_string
/*!
	To perform the bool to string function it is necessary to peform a divide up operation
	\param dividend What to divide
	\param divisor What to divide with
	\return The resulting updivided value
*/
std::size_t divide_up(std::size_t dividend, std::size_t divisor);

//! Converts a boolean vector to a string
/*!
	For file saving or when wanting to printing a boolean deque with additional information this function converts it to a readable string
	\param boolvector Boolean deque to convert
	\return A textual representation of the boolean deque as string
*/
std::string bool_to_string(std::deque<bool> const& boolvector);

//! Saves a boolean deque to file
/*!
	Saves a boolen deque to a newly created file
	\param file_name The name of the new file
	\param in Boolean deque to save to the file
*/
void write_bool_deque_to_file(const std::string& file_name, const std::deque<bool>& in);

//! Reads as boolean deque from file
/*!
	Reads a file and converts the cotent to a boolean deque for further processing
	\param file_name Name of the file to read from
	\return The read boolean deque
*/
std::deque<bool> read_bool_deque_from_file(const std::string& file_name);

//! Pops n elements from the front of a boolean deque
/*!
	During processing a boolen deque, values are removed from the front when not needed anymore
	\param in The boolen deque to pop values from
	\param n Number of objects to remove from the front
*/
void pop_n_from_front(std::deque<bool>& in, const unsigned int& n);

}
}

#endif