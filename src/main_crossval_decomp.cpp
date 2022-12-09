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

	// DECODER TEST VECTORS

	// INPUT VECTORS
	// Decoder input vectors are stored in the decoder_input folder, with extension .config+124. These vectors are designed to stress the decoder implementation.
	// Each input vector is a concatenation of input elements. Each input element will be of one of the two following types:
	// Type 1: Received compressed packet. The format of this element type is as follows:
	//    1. Packet Reception Byte
	//        - The LSB of this byte shall be zero.
	//        - The values of the other bits in this byte are undefined and shall be ignored.
	//    2. Received Packet Length (32 bits, unsigned, big endian).
	//        - The compressed packet length, in bits.
	//    3. Compressed Packet Bitstream (variable length). Note that:
	//        - If the Received Packet Length is not a multiple of 8, the last byte of the compressed data is padded so that the next element begins at the next byte boundary.
	//        - The value of these padding bits is not defined. I.e., the padding bits shall be ignored.
	//        - If the Received Packet Length is a multiple of 8, no padding bits shall be included.
	// Type 2: Lost packet. The format of this element type is as follows:
    //    1. Packet Reception Byte (8 bits).
    //        – The LSB of this byte shall be one.
    //        – The values of the other bits in this byte are undefined and shall be ignored.

	// 

	// OUTPUT VECTORS
	// 

	// Define variables
	std::string sequence_number;
	std::deque<bool> decoder_sequence_raw;
	std::deque<bool> decoder_output_reference;
	std::deque<bool> decoder_output;
	std::unique_ptr<unsigned int> offset;
	std::deque<bool>::iterator packet_reception_first;
	std::deque<bool>::iterator packet_reception_last;
	std::deque<bool> packet_reception_byte;
	std::unique_ptr<bool> packet_type;
	std::unique_ptr<unsigned int> received_packet_length;
	std::unique_ptr<unsigned int> bit_shift;
	std::deque<bool>::iterator received_packet_first;
	std::deque<bool>::iterator received_packet_last;
	std::deque<bool> received_packet;
	std::unique_ptr<unsigned int> input_packet_size;
	std::deque<bool> output_vector;
	std::deque<bool> status_byte;

	// Discover all files in the crossvalidation folder
	// ./crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/decoder_input
	const std::filesystem::path path{"crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/decoder_input"};
    for (auto const& dir_entry : std::filesystem::directory_iterator{path}){
		// Extract the sequence number from the path
		sequence_number = dir_entry.path().string().substr(103, 5);

		// Read encoder input test vector file and convert to boolean deque
		decoder_sequence_raw = pocketplus::utils::read_bool_deque_from_file(dir_entry.path().string());
		//sequence_number = "16183";
		//decoder_sequence_raw = pocketplus::utils::read_bool_deque_from_file("crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/decoder_input/decoder_sequence_" + sequence_number + ".124+config");

		//if(sequence_number == "1"){
		//	std::cout << "Decoder input sequence:" << std::endl;
	    //	pocketplus::utils::print_vector(decoder_sequence_raw);
		//}

		//if(decoder_sequence_raw.size() < 100){

		//std::cout << "Decoder input sequence:" << std::endl;
	    //pocketplus::utils::print_vector(decoder_sequence_raw);
		//std::cout << "Decoder input sequence size: " << decoder_sequence_raw.size() << std::endl;

		// Read decoder output reference file and convert to boolean deque
		decoder_output_reference = pocketplus::utils::read_bool_deque_from_file("crossvalidation_124x0b1_release_2022-03-08/crossvalidation_ccsds124x0b1/decoder_output/decoder_sequence_" + sequence_number + ".raw+large_f");
		//std::cout << "Decoder output reference size: " << decoder_output_reference.size() << std::endl;
		//std::cout << "Decoder output reference:" << std::endl;
		//pocketplus::utils::print_vector(decoder_output_reference);

		//if(decoder_sequence_raw.size() < 100){
		//	std::cout << "Sequence number: " + sequence_number << std::endl;
		//}

		// Initialize the input vector size to zero
		input_packet_size = std::make_unique<unsigned int>(0);

		// Define decompressor object
		std::unique_ptr<pocketplus::decompressor::PocketPlusDecompressor> decompressor;

		// Empty the decoder_output
		decoder_output = std::deque<bool>();

		// Initialize bit offset counter
		offset = std::make_unique<unsigned int>(0);

		// Loop until file end is reached
		auto i = 0;
		received_packet_last = decoder_sequence_raw.begin();
		while(received_packet_last != decoder_sequence_raw.end()){
			i++;
			//if(i == 18)
			//	std::cout << "t: " << i << std::endl;

			// Read packet flag configuration byte
			packet_reception_first = decoder_sequence_raw.begin() + *offset;
			// Increase offset by one byte
			*offset += 8;
			packet_reception_last = decoder_sequence_raw.begin() + *offset;
			packet_reception_byte = std::deque<bool>(packet_reception_first, packet_reception_last);
			//pocketplus::utils::print_vector(packet_reception_byte);

			// Read packet type
			// If the packet reception byte LSB is zero, it is a Type 1 received compressed packet
			// Else, it is a Type 2 lost packet
			packet_type = std::make_unique<bool>(packet_reception_byte.at(7));
			//std::cout << "packet_status: " << *packet_type << std::endl;

			// Process packet type
			// Type 2: Lost Packet
			if(*packet_type == 1){
				//return -1;
				status_byte = std::deque<bool>{0, 0, 0, 0, 0, 0, 1, 0}; // 0x02
				decoder_output.insert(decoder_output.end(), status_byte.begin(), status_byte.end());
				continue;
			}
			// From here on we assume to have a Type 1 packet

			// Read received packet length from file
			received_packet_length = std::make_unique<unsigned int>(0);
			bit_shift = std::make_unique<unsigned int>(0);
			for(auto it = decoder_sequence_raw.begin() + *offset + 31; it >= (decoder_sequence_raw.begin() + *offset); it--, *bit_shift += 1){
				if(*it){
					*received_packet_length |= 1 << *bit_shift;
				}
			}
			//std::cout << "received_packet_length: " << *received_packet_length << std::endl;
			*offset += 32;
			if(*received_packet_length > decoder_sequence_raw.size()){
				break;
			}
			// Read received packet data
			if(*offset < decoder_sequence_raw.size()){
				received_packet_first = decoder_sequence_raw.begin() + *offset;
			}
			else{
				received_packet_first = decoder_sequence_raw.end();
			}
			if((*offset + *received_packet_length) < decoder_sequence_raw.size()){
				// Increase offset by received packet length
				*offset += *received_packet_length;
				received_packet_last = decoder_sequence_raw.begin() + *offset;
			}
			else{
				received_packet_last = decoder_sequence_raw.end();
			}
			if(received_packet_first == received_packet_last){
				//std::cout << "No input vector left" << std::endl;
				//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
				//return -1;
				break;
			}
			received_packet = std::deque<bool>(received_packet_first, received_packet_last);
			//std::cout << "received_packet size: " << received_packet.size() << std::endl;
			//pocketplus::utils::print_vector(received_packet);

			if(received_packet.size() != *received_packet_length){
				//std::cout << "Incomplete input vector, not enough bits left" << std::endl;
				break;
			}

			// Add zero padding to the offset for the next packet
			if((*received_packet_length % 8) != 0){
				if((*offset + (8 - (*received_packet_length % 8))) < decoder_sequence_raw.size()){
					*offset += 8 - (*received_packet_length % 8);
					received_packet_last = decoder_sequence_raw.begin() + *offset;
				}
				else{
					received_packet_last = decoder_sequence_raw.end();
				}
			}

			// Check if a context has already been established
			if(!decompressor){
				// Read input packet size from the received packet
				try{
					*input_packet_size = pocketplus::decompressor::PocketPlusDecompressor::get_input_vector_length(received_packet);
					//std::cout << "input_packet_size: " << *input_packet_size << std::endl;
				}
				catch(const std::exception& e){
					//std::cerr << "Error: " << e.what() << std::endl;
					//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
					//return -1;
					status_byte = std::deque<bool>{0, 0, 0, 0, 0, 0, 0, 1}; // 0x01
					decoder_output.insert(decoder_output.end(), status_byte.begin(), status_byte.end());
					break;
				}
				if(*input_packet_size == 0){
					//return -1;
					status_byte = std::deque<bool>{0, 0, 0, 0, 0, 0, 0, 1}; // 0x01
					decoder_output.insert(decoder_output.end(), status_byte.begin(), status_byte.end());
					continue;
				}

				// Create decompressor object if it does not exist
				try{
					decompressor = std::make_unique<pocketplus::decompressor::PocketPlusDecompressor>(*input_packet_size);
				}
				catch(const std::exception& e){
					//std::cerr << "Error: " << e.what() << std::endl;
					//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
					//return -1;
					status_byte = std::deque<bool>{0, 0, 0, 0, 0, 0, 0, 1}; // 0x01
					decoder_output.insert(decoder_output.end(), status_byte.begin(), status_byte.end());
					*input_packet_size = 0;
					break;
				}
			}

			// Decompress received packet
			try{
				output_vector = decompressor->decompress(received_packet);
			}
			catch(const std::exception& e){
				//std::cerr << "Error: " << e.what() << std::endl;
				//std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
				status_byte = std::deque<bool>{0, 0, 0, 0, 0, 0, 0, 1}; // 0x01
				decoder_output.insert(decoder_output.end(), status_byte.begin(), status_byte.end());
				//*input_packet_size = 0;
				//return -1;
				continue;
			}
			status_byte = std::deque<bool>{0, 0, 0, 0, 0, 0, 0, 0}; // 0x00
			decoder_output.insert(decoder_output.end(), status_byte.begin(), status_byte.end());
			pocketplus::utils::zero_stuffing(output_vector);
			//pocketplus::utils::print_vector(output_vector);
			std::move(output_vector.begin(), output_vector.end(), std::back_inserter(decoder_output));
		}
		// Append the input packet size as 32 bit unsigned integer
		for(int j = 31; j >= 0; j--){
			decoder_output.emplace_back((*input_packet_size >> j) & 1);
		}

		// Compare the encoder output with the reference file
		//std::cout << "Decoder output reference size: " << decoder_output_reference.size() << std::endl;
		//std::cout << "Decoder output reference:" << std::endl;
		//pocketplus::utils::print_vector(decoder_output_reference);
		//std::cout << "Decoder output size: " << decoder_output.size() << std::endl;
		//std::cout << "Decoder output:" << std::endl;
		//pocketplus::utils::print_vector(decoder_output);
		std::cout << "Sequence: " << sequence_number << " Equality: " << (decoder_output == decoder_output_reference) << std::endl;
		//break;
		//if((encoder_output != encoder_output_reference))// && (encoder_output_reference.size() < 600))
		//	std::cout << "Sequence:" << sequence_number << " Equality: " << (encoder_output == encoder_output_reference) << std::endl;
		//return -1;

		//}
    }
}