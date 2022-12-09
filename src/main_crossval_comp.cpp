#include <iterator>

#include "pocketpluscompressor.h"
#include "pocketplusdecompressor.h"
#include "pocketplusutils.h"

//! Main function, called on application startup
/*!
	This main function provied a simple setup to test the compression and decompression of consecutive data packets.
	\param argc Number of call arguments
	\param argv Content of the call arguments
	\return Execution result code
*/
int main(__attribute__((unused))int argc, __attribute__((unused)) char* argv[]){

	// CCSDS yellow book cross validation tests

	// ENCODER TEST VECTORS

	// INPUT VECTORS
	// Encoder input vectors are stored in the encoder_input folder, with extension .config+raw. These input vectors
	// are designed to stress the encoder implementation. Each vector represents a sequence of uncompressed packets, to be
	// processed by the encoder. The expected output for each input vector is described in the next section. The format of each
	// input vector is as follows:
	// 1. large_f, i.e., number of bits per packet (32 bits, unsigned, big endian).
	// 2. large_m_0, i.e., the initial mask (large_f bits + zero padding).
	// 3. For each input packet in the test vector:
	//    1. Flag Configuration Byte (8 bits).
	//    2. Packet content, uncompressed (large_f bits + zero padding).

	// The syntax of each bit in a Flag Configuration Byte is as follows:
	// bit 1 (MSB): reserved - value to be ignored
	// bit 2: send full mask flag f - 0 disables it, 1 enables it
	// bit 3: update mask flag p - 0 disables it, 1 enables it
	// bit 4: send reference packet flag r - 0 disables it, 1 enables it
	// bits 5, 6, 7, 8: robustness level R. Expressed as a 4-bit unsigned big-endian value
	//    i.e., 8*bit5 + 4*bit6 + 2*bit7 + bit 8
	//    Note that 3 bits would be enough to test all values allowed by the standard, but this allows to test invalid ones as well.

	// OUTPUT VECTORS
	// For each input vector in the encoder_input folder, an output vector is available in the encoder_output folder.
	// The name of the output vectors is identical to those of the input vectors, substituting the input with extension .124.
	// The format of each output vector is a concatenation of compressed packets produced by the encoder.
	// For an input vector containing invalid parameters at packet t = t_error (for the first time), the associated output
    // vector is the concatenation of all compressed packets for t = 0, . . . , t_error - 1. In particular, the associated
    // output vector is an empty file when t_error is zero.
	// Compressed packets are byte-aligned by adding padding bits as necessary to reach the next byte boundary. I.e., at
    // most 7 padding bits are added per compressed packet.
	// Output padding bits are set to zero.

	// Define variables
	std::deque<bool> encoder_sequence_raw;
	std::unique_ptr<pocketplus::compressor::PocketPlusCompressor> compressor;
	std::unique_ptr<unsigned int> input_vector_length;
	std::unique_ptr<unsigned int> bit_shift;
	std::unique_ptr<unsigned int> zero_padding;
	std::deque<bool>::const_iterator padding_first;
	std::deque<bool>::const_iterator padding_last;
	std::deque<bool> padding;
	std::deque<bool>::const_iterator initial_mask_first;
	std::deque<bool>::const_iterator initial_mask_last;
	std::deque<bool> initial_mask;
	std::deque<bool>::const_iterator flag_configuration_first;
	std::deque<bool>::const_iterator flag_configuration_last;
	std::deque<bool> flag_configuration_byte;
	std::unique_ptr<bool> send_mask_flag;
	std::unique_ptr<bool> new_mask_flag;
	std::unique_ptr<bool> uncompressed_flag;
	std::unique_ptr<unsigned int> robustness_level;
	std::unique_ptr<unsigned int> offset;
	std::deque<bool>::const_iterator input_packet_first;
	std::deque<bool>::const_iterator input_packet_last;
	std::deque<bool> input_packet;
	std::deque<bool> output_vector;
	std::deque<bool> encoder_output;
	std::deque<bool> encoder_output_reference;

	// Discover all files in the crossvalidation folder
	// ./crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/encoder_input
	const std::filesystem::path path{"crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/encoder_input"};
	std::string sequence_number;
    for (auto const& dir_entry : std::filesystem::directory_iterator{path}) 
    {
        //std::cout << dir_entry.path() << std::endl;
		sequence_number = dir_entry.path().string().substr(103, 4);
		//std::cout << sequence_number << std::endl;

		// Read encoder input test vector file and convert to boolean deque
		encoder_sequence_raw = pocketplus::utils::read_bool_deque_from_file(dir_entry.path().string());
		//std::cout << "Sequence:" << sequence_number << std::endl;
		//sequence_number = "4814";
		//encoder_sequence_raw = pocketplus::utils::read_bool_deque_from_file("crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/encoder_input/encoder_sequence_" + sequence_number + ".raw+config");

		//std::cout << "Encoder input sequence:" << std::endl;
	    //pocketplus::utils::print_vector(encoder_sequence_raw);
		//std::cout << "Encoder input sequence size: " << encoder_sequence_raw.size() << std::endl;

		// Read encoder output reference file and convert to boolean deque
		encoder_output_reference = pocketplus::utils::read_bool_deque_from_file("crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/encoder_output/encoder_sequence_" + sequence_number + ".124");
		//std::cout << "Encoder output reference size: " << encoder_output_reference.size() << std::endl;
		//std::cout << "Encoder output reference:" << std::endl;
		//pocketplus::utils::print_vector(encoder_output_reference);

		// Create compressor object
		compressor = std::make_unique<pocketplus::compressor::PocketPlusCompressor>();

		// Empty the encoder_output
		encoder_output.clear();

		// Read input vector length from file
		input_vector_length = std::make_unique<unsigned int>(0);
		bit_shift = std::make_unique<unsigned int>(0);
		for(auto it = encoder_sequence_raw.begin() + 31; it >= encoder_sequence_raw.begin(); it--, *bit_shift += 1){
			if(*it){
				*input_vector_length |= 1 << *bit_shift;
			}
		}
		//std::cout << "input_vector_length: " << *input_vector_length << std::endl;

		try{
			// Set input vector length
			compressor->set_input_vector_length(*input_vector_length);
		}
		catch(const std::exception& e){
			//std::cerr << "Error: " << e.what() << std::endl;
			//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
			//return -1;
			//if((encoder_output != encoder_output_reference))// && (encoder_output_reference.size() < 600))
				std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
			continue;
		}

		// Calculate zero padding bit count for byte alignment
		if((*input_vector_length % 8) == 0){
			zero_padding = std::make_unique<unsigned int>(0);
		}
		else{
			zero_padding = std::make_unique<unsigned int>(8 - (*input_vector_length % 8));
		}
		//std::cout << "zero_padding: " << *zero_padding << std::endl;

		// Check if initial mask padding is all zero
		if(*zero_padding != 0){
			padding_first = encoder_sequence_raw.begin() + 32 + *input_vector_length;
			padding_last = encoder_sequence_raw.begin() + 32 + *input_vector_length + *zero_padding;
			padding = std::deque<bool>(padding_first, padding_last);
			if(!std::all_of(padding.begin(), padding.end(), [](int i) { return i==0; })){
				//std::cout << "Invalid mask zero padding" << std::endl;
				//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
				//return -1;
				//if((encoder_output != encoder_output_reference))// && (encoder_output_reference.size() < 600))
					std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
				continue;
			}
		}
		// Read initial mask vector
		initial_mask_first = encoder_sequence_raw.begin() + 32;
		initial_mask_last = encoder_sequence_raw.begin() + 32 + *input_vector_length;
		initial_mask = std::deque<bool>(initial_mask_first, initial_mask_last);
		//std::cout << "initial_mask size: " << initial_mask.size() << std::endl;
		//pocketplus::utils::print_vector(initial_mask);

		// Set initial mask
		try{
			compressor->set_initial_mask(initial_mask);
		}
		catch(const std::exception& e){
			//std::cerr << "Error: " << e.what() << std::endl;
			//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
			//return -1;
			//if((encoder_output != encoder_output_reference))// && (encoder_output_reference.size() < 600))
				std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
			continue;
		}

		// Initialize bit offset counter
		offset = std::make_unique<unsigned int>(32 + *input_vector_length);

		// Loop until file end is reached
		//auto i = 0;
		while(input_packet_last != encoder_sequence_raw.end()){
			//if(i == 255)
			//std::cout << "t: " << i << std::endl;
			//i++;
			// Increse offset by the zero padding for byte alignment
			*offset += *zero_padding;
			// Read packet flag configuration byte
			flag_configuration_first = encoder_sequence_raw.begin() + *offset;
			// Increase offset by one byte
			*offset += 8;
			flag_configuration_last = encoder_sequence_raw.begin() + *offset;
			flag_configuration_byte = std::deque<bool>(flag_configuration_first, flag_configuration_last);
			//std::cout << "flag_configuration_byte size: " << flag_configuration_byte.size() << std::endl;
			//pocketplus::utils::print_vector(flag_configuration_byte);
			send_mask_flag = std::make_unique<bool>(flag_configuration_byte.at(1));
			new_mask_flag = std::make_unique<bool>(flag_configuration_byte.at(2));
			uncompressed_flag = std::make_unique<bool>(flag_configuration_byte.at(3));
			robustness_level = std::make_unique<unsigned int>(8*flag_configuration_byte.at(4) + 4*flag_configuration_byte.at(5) + 2*flag_configuration_byte.at(6) + 1*flag_configuration_byte.at(7));
			//std::cout << "send_mask_flag: " << *send_mask_flag << std::endl;
			//std::cout << "new_mask_flag: " << *new_mask_flag << std::endl;
			//std::cout << "uncompressed_flag: " << *uncompressed_flag << std::endl;
			//std::cout << "robustness_level: " << *robustness_level << std::endl;

			// Read input packet data
			if(*offset < encoder_sequence_raw.size()){
				input_packet_first = encoder_sequence_raw.begin() + *offset;
			}
			else{
				input_packet_first = encoder_sequence_raw.end();
			}
			
			if((*offset + *input_vector_length) < encoder_sequence_raw.size()){
				// Increase offset by input vector length
				*offset += *input_vector_length;
				input_packet_last = encoder_sequence_raw.begin() + *offset;

				// Check if input packet padding is all zero
				if((*zero_padding != 0) && ((*offset + *zero_padding) < encoder_sequence_raw.size())){
					padding_first = encoder_sequence_raw.begin() + *offset;
					padding_last = encoder_sequence_raw.begin() + *offset + *zero_padding;
					padding = std::deque<bool>(padding_first, padding_last);
					if(!std::all_of(padding.begin(), padding.end(), [](int i) { return i==0; })){
						//std::cout << "Invalid input packet zero padding" << std::endl;
						//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
						//return -1;
						break;
					}
				}

				// Check if tailing padding is all zero (if no full config byte is present after the input packet)
				if((encoder_sequence_raw.size() - *offset) < 8){
					padding_first = encoder_sequence_raw.begin() + *offset;
					padding_last = encoder_sequence_raw.begin() + *offset + (encoder_sequence_raw.size() - *offset);
					padding = std::deque<bool>(padding_first, padding_last);
					if(!std::all_of(padding.begin(), padding.end(), [](int i) { return i==0; })){
						//std::cout << "Invalid tailing zero padding" << std::endl;
						//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
						//return -1;
						break;
					}
				}
			}
			else{
				input_packet_last = encoder_sequence_raw.end();
			}
			if(input_packet_first == input_packet_last){
				//std::cout << "No input vector left" << std::endl;
				//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
				//return -1;
				break;
			}
			input_packet = std::deque<bool>(input_packet_first, input_packet_last);
			//std::cout << "input_packet size: " << input_packet.size() << std::endl;
			//pocketplus::utils::print_vector(input_packet);

			// Compress input packet
			try{
				//if(i >= 60){
				//	//if(encoder_output.size() > 1400){
				//	std::cout << encoder_output.size() << std::endl;
				//}
				output_vector = compressor->compress(input_packet, *robustness_level, *new_mask_flag, *send_mask_flag, *uncompressed_flag);
			}
			catch(const std::exception& e){
				//std::cerr << "Error: " << e.what() << std::endl;
				//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
				//return -1;
				break;
			}
			pocketplus::utils::zero_stuffing(output_vector);
			std::move(output_vector.begin(), output_vector.end(), std::back_inserter(encoder_output));
			//std::cout << "Encoder output:" << std::endl;
			//pocketplus::utils::print_vector(encoder_output);
		}
		// Compare the encoder output with the reference file
		//std::cout << "Encoder output size: " << encoder_output.size() << std::endl;
		//std::cout << "Encoder output reference:" << std::endl;
		//pocketplus::utils::print_vector(encoder_output_reference);
		//pocketplus::utils::print_vector(encoder_output);
		//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
		//break;
		//if((encoder_output != encoder_output_reference))// && (encoder_output_reference.size() < 600))
			std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
    }
}