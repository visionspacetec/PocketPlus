#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <deque>
#include <memory>
#include <iterator>

#include "pocketplusutils.h"

namespace pocketplus {
namespace decompressor {

//! PocketPlusDecompressor class
/*!
	This class implements the implicitly provided decompression for the CCSDS draft standard for housekeeping telemetry data compression
*/
class PocketPlusDecompressor{
	//! Private Hamming weight calculation function
	/*! 
		Counts the number of ones in vector range, defined by the two input parameters
		\param start Itterator for the start position
		\param stop Itterator for the stop position
		\return Returns the unsigned integer number of ones in the selected range
	*/
	unsigned int hamming_weight_in_range(std::deque<bool>::iterator start, std::deque<bool>::iterator stop);
	
	//! Private function to reverse a boolean deque
	/*!
		Simple helper function to revert a boolen deque
		\param a Boolen deque to revert
		\return The reverted boolen deque
	*/
	std::deque<bool> reverse(const std::deque<bool>& a);

	//! Private function to undo the run length encoding
	/*!
		Reverts the COUNT encoding function while looking for the run length encoding termination sequence
		\param in The input vector to extract the COUNT encoded values from
		\param out Decoded values extracted from the input
		\param it Processing itterator
	*/
	void undo_rle(std::deque<bool>& in, std::deque<bool>& out, std::deque<bool>::iterator& it);

	//! Constant minimum supported input vector size
	std::unique_ptr<const unsigned int> minimum_size;

	//! Size of the input boolen deque, before trying to extract the front compressed packet from it
	std::unique_ptr<unsigned int> input_vector_size_before_processing;
	//! Counter for successful decompressions
	std::unique_ptr<unsigned int> t; 
	//! Input vector length, denoted F in the standard
	std::unique_ptr<unsigned int> input_vector_length;
	//! Read robustness level from the currently processed packet, denoted R_t in the standard
	std::unique_ptr<unsigned int> robustness_level;
	//! Read d_t of the currently processed packet 
	std::unique_ptr<bool> d_t;
	//! Deque of deques to save the previous input vectors, I
	std::deque<std::deque<bool>> input_vector;
	//! Deque of deques to save the previous mask vectors, M
	std::deque<std::deque<bool>> mask_vector;
	//! Deque of deques to save the previous mask change vectors, D
	std::deque<std::deque<bool>> change_vector;
	//! Deque used to check the compliance with the send mask flag requirement
	std::deque<bool> check_send_changes_flag;

	public:
		//! PocketPlusDecompressor default constructor
		/*!
			The PocketPlusDecompressor performs the stateful decompression according to the CCSDS draft standard for housekeeping telemetry data compresison
			\param vector_length Defines the expected input vector length of the compressed data packets
		*/
		PocketPlusDecompressor(std::unique_ptr<unsigned int>& vector_length){
			if ((*vector_length < 1) || (*vector_length > 65535)){
				throw std::out_of_range("1 <= input_vector_length <= 2^16-1 (65535");
			}
			input_vector_length = std::make_unique<unsigned int>(*vector_length);
			minimum_size = std::make_unique<const unsigned int>(8);
			input_vector_size_before_processing = std::make_unique<unsigned int>(0);
			t = std::make_unique<unsigned int>(0);
			check_send_changes_flag = {1, 0, 0, 0, 0, 0};
			std::deque<bool> initial_mask_vector;
			initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0 // ############ ToDo: Make initial mask user defined
			mask_vector.push_back(initial_mask_vector);
		}

		//! Public function to decompress a given boolen deque
		/*!
			The only public function this class offers tries to decompress a single data packet from the given deque and returns the remaining bits
			\param input Boolean deque to extract compressed packet from
			\return Remaining bits in the input boolean deque after extracting the front packet from it
		*/
		std::deque<bool> decompress(std::deque<bool>& input);
};

};
};

#endif