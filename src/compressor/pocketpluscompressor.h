#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <deque>
#include <memory>
#include <algorithm>
#include <cmath>
#include <gtest/gtest_prod.h>

#include "pocketplusutils.h"

namespace pocketplus {
namespace compressor {

//! PocketPlusCompressor class
/*!
	This class implements the latest CCSDS draft standard for housekeeping telemetry data compression
*/
class PocketPlusCompressor{
	FRIEND_TEST(count, InputValid1);
	FRIEND_TEST(count, InputTooSmall);
	FRIEND_TEST(count, InputTooLarge);
	FRIEND_TEST(bit_extraction, InputValid);
	FRIEND_TEST(bit_extraction, InputInvalid);

	//! Private COUNT encoding function implementation
	/*!
		Private function to set the input vector length of this class
		\param a Unsigned integer to COUNT encode
		\return The encoded value as boolen deque
	*/
	std::deque<bool> count(const unsigned int& a);

	//! Private Run Length Encoding (RLE) function
	/*!
		Private run length encoding function in compliance with paragraph 5.3.1.2
		\param a Boolean deque to run length encode
		\return The run length encoding of a
	*/
	std::deque<bool> run_length_encoding(const std::deque<bool>& a);

	//! Private function to perform the bit extraction from two boolean deques
	/*!
		Private function to perform the bit extraction in accordance to paragraph 5.3.1.3 
		\param a Boolean deque telling the position of the extracted bits
		\param b Boolean deque from which the bits are extracted
		\return Bits extracted returned as boolen deque
	*/
	std::deque<bool> bit_extraction(const std::deque<bool>& a, const std::deque<bool>& b);

	//! Private function to calculate the Hamming weight for a boolean deque
	/*!
		Private Hamming weight calculation, this is equal to the number of ones in the given vector
		\param a Boolean deque to count number of ones inside
		\return Unique pointer to unsigned integer returning the number of ones found
	*/
	std::unique_ptr<unsigned int> hamming_weight(const std::deque<bool>& a);

	//! Private function that reverses the given boolean deque
	/*!
		Private function that reverses the given boolean deque, denoted < A > in the standard
		\param a Boolean deque to reverse
		\return The reversed boolean deque
	*/
	std::deque<bool> reverse(const std::deque<bool>& a);

	//! Private function that inverts the booleans in the given boolean deque
	/*!
		Private function that inverts the booleans in the given boolean deque. All bits are toggled by this function, denoted ~A in the standard.
		\param a Boolean deque to invert
		\return The inverted boolen deque
	*/
	std::deque<bool> inverse(const std::deque<bool>& a);

	//! Constant minimum allowed robustness level
	std::shared_ptr<const unsigned int> robustness_level_min;
	//! Constant maximum allowed robustness level
	std::shared_ptr<const unsigned int> robustness_level_max;

	//! Internal time counter which is equavalent to a processed packet counter
	std::shared_ptr<unsigned int> t;
	//! The currently set input vector length F
	std::shared_ptr<unsigned int> input_vector_length;
	//! The input vector length F, encoded usign the COUNT function
	std::deque<bool> input_vector_length_count;
	//! The initial mask vector
	std::deque<bool> initial_mask_vector;
	//! New mask flag p_t vector
	std::deque<bool> mask_flag;
	//! Previous input vector I_t-1
	std::deque<bool> input_old;
	//! New mask vector M_t
	std::deque<bool> mask_new;
	//! Old mask vector M_t-1
	std::deque<bool> mask_old;
	//! Previous mask build vector
	std::deque<bool> mask_build_old;
	//! New mask build vector
	std::deque<bool> mask_build_new;
	//! Mask change vector D
	std::deque<std::deque<bool>> mask_change_vector;
	//! Initial mask change vector D_0
	std::deque<bool> mask_change_0;
	//! The run length encoded mask change vector
	std::deque<bool> rle_mask_change;
	//! Bit extraciton vector from the input vector relative to the current mask
	std::deque<bool> input_mask_bit_extraction;
	//! X_t is the reverse of the mask change vector
	std::deque<bool> X_t;
	//! Bit extraction of the inverted mask vector from the reversed X_t vector
	std::deque<bool> y_t;
	//! e_t is a flag to signal the robustness level and if y_t or X_t are all zeros
	std::deque<bool> e_t;
	//! k_t is either empty or containing y_t
	std::deque<bool> k_t;
	//! c_t signals mask resets in relation to the effective robustness level
	std::deque<bool> c_t;
	//! Run length encoded X_t
	std::deque<bool> X_t_rle;
	//! Counte rof no mask changes
	std::unique_ptr<unsigned int> no_mask_changes;
	//! Consecutive occurences of no mask changes, from the first iteration not covered by the minimum reqired robustness level
	std::unique_ptr<unsigned int> C_t;
	//! Effective robustness level
	std::unique_ptr<unsigned int> V_t;
	//! Effective robustness level in four bit representation
	std::deque<bool> V_t_bit_4;
	//! Run length encoded mask XORed with itself
	std::deque<bool> mask_shifted_rle;
	//! First part of the output vector h_t with the mask change information
	std::deque<bool> first_binary_vector;
	//! Second part of the output vector q_t with information about the entire mask vector
	std::deque<bool> second_binary_vector;
	//! Second part of the output vector u_t with either unpredictable bits or the original input I_t
	std::deque<bool> third_binary_vector;
	//! Completely assembled output vector with h_t, q_t and u_t concatenated
	std::deque<bool> output;

	public:
		//! PocketPlusCompressor default constructor
		/*!
			The PocketPlusCompressor constructor has no arguments, it is configured after creation
		*/
		PocketPlusCompressor(){
			robustness_level_min = std::make_shared<const unsigned int>(0);
			robustness_level_max = std::make_shared<const unsigned int>(7);
			t = std::make_shared<unsigned int>(0);
			/*
			input_vector_length = std::make_unique<unsigned int>(*vector_length); // F // User defined value
			input_vector_length_count = count(*input_vector_length);
			initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0 // ############ ToDo: Make initial mask user defined
			mask_new = initial_mask_vector;
			input_old.assign(*input_vector_length, 0);
			mask_old.assign(*input_vector_length, 0); // M_t
			mask_build_old.assign(*input_vector_length, 0); // B_t // B_0 = 0
			mask_build_new.assign(*input_vector_length, 0); // B_t // B_0 = 0
			mask_change_0.assign(*input_vector_length, 0); 
			*/
		}

		//! PocketPlusCompressor copy constructor
		/*!
			This copy constructor takes a PocketPlusCompressor object reference and creates an new object as a copy of it
			\param old Reference to the PocketPlusCompressor object to copy
		*/
		PocketPlusCompressor(const PocketPlusCompressor& old){
			robustness_level_min = std::make_shared<const unsigned int>(0);
			robustness_level_max = std::make_shared<const unsigned int>(7);
			t = std::make_shared<unsigned int>(*old.t);
			input_vector_length = std::make_shared<unsigned int>(*old.input_vector_length);
			input_vector_length_count = old.input_vector_length_count;
			initial_mask_vector = old.initial_mask_vector;
			mask_flag = old.mask_flag;
			input_old = old.input_old;
			mask_new = old.mask_new;
			mask_old = old.mask_old;
			mask_build_old = old.mask_build_old;
			mask_build_new = old.mask_build_new;
			mask_change_vector = old.mask_change_vector;
			mask_change_0 = old.mask_change_0;
			no_mask_changes = std::make_unique<unsigned int>(*old.no_mask_changes);
			C_t = std::make_unique<unsigned int>(*old.C_t);
			V_t = std::make_unique<unsigned int>(*old.V_t);
		}

		//! Public function to set the input vector length
		/*!
			Public function to set the input vector length of this class
			\param vector_length New input vector length
		*/
		void set_input_vector_length(const unsigned int& vector_length);

		//! Public function to get the input vector length
		/*!
			\return The currently set unsigned integer input vector length
		*/
		unsigned int get_input_vector_length();

		//! Public function to perform the actual compression
		/*!
			Public function to perform the compression of a given boolean deque
			\param input_new The new input data packet
			\param robustness_level Robustness level to be used for this packet
			\param new_mask_flag New mask flag to be used for this packet
			\param send_mask_flag Send mask flag for this data packet
			\param uncompressed_flag Uncompressed flag for this data packet
			\return The compressed data packet as boolean deque
		*/
		std::deque<bool> compress(
			const std::deque<bool>& input_new, 
			const unsigned int& robustness_level,
			const bool& new_mask_flag,
			const bool& send_mask_flag,
			const bool& uncompressed_flag
		);
};

};
};

#endif