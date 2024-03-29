#include "pocketplusdecompressor.h"

using namespace pocketplus::decompressor;

// Hamming weight - Number of ones in vector
unsigned int PocketPlusDecompressor::hamming_weight(const std::deque<bool>& a){
	auto output = std::make_unique<unsigned int>(0);
	for(auto it = a.begin(); it != a.end(); it++){
		*output += *it;
	}
	return *output;
}

// Hamming weight in range - Number of ones in boolean deque in the given range
unsigned int PocketPlusDecompressor::hamming_weight_in_range(std::deque<bool>::iterator start, std::deque<bool>::iterator stop){
	unsigned int output = 0;
	for(auto i = start; i <= stop; i++){
		output += *i;
	}
	return output;
}

// Reverse deque helper function, denoted < A > in the standard
std::deque<bool> PocketPlusDecompressor::reverse(const std::deque<bool>& a){
	std::deque<bool> output_vector;
	for(auto i = a.rbegin(); i != a.rend(); i++ ){
		output_vector.emplace_back(*i);
	}
	return output_vector;
}

// Undo run length encoding
void PocketPlusDecompressor::undo_rle(std::deque<bool>& in, std::deque<bool>& out, std::deque<bool>::iterator& it){
	if(it == in.end()){
		throw std::invalid_argument("Revert of COUNT(X) failed");
	}
	while(!((*it == 1) && (*(it + 1) == 0))){ // '1' '0' indicates the end of the RLE
		if(it == in.end()){
			throw std::invalid_argument("Revert of COUNT(X) failed");
		}
		// Revert COUNT operation
		if(*it == 0){
			out.emplace_back(1);
			it += 1;
			in.pop_front();
		}
		else if(((it + 1) != in.end()) && ((it + 2) != in.end()) && ((it + 3) != in.end()) && ((it + 4) != in.end())){
			if((*it == 1) && (*(it+1) == 1) && (*(it+2) == 0)){
				it += 3;
				pocketplus::utils::pop_n_from_front(in, 3);
				// Undo BIT_5(A - 2)
				if((in.size() < 5) || in.empty()){
					throw std::invalid_argument("Revert of COUNT(X) failed");
				}
				auto count_tmp = std::make_unique<unsigned int>(0);
				auto bit_shift = std::make_unique<unsigned int>(0);
				for(auto ite = it + 4; ite >= it; ite--, *bit_shift += 1){
					if(*ite){
						*count_tmp |= 1 << *bit_shift;
					}
				}
				*count_tmp += 1;
				it += 5;
				pocketplus::utils::pop_n_from_front(in, 5);
				for(unsigned int i = 0; i < *count_tmp; i++){
					out.emplace_back(0);
				}
				out.emplace_back(1);
			}
			else if((*it == 1) && (*(it+1) == 1) && (*(it+2) == 1)){
				it += 3;
				pocketplus::utils::pop_n_from_front(in, 3);
				if((in.size() < 6) || in.empty()){
					throw std::invalid_argument("Revert of COUNT(X) failed");
				}
				// Undo BIT_E(A - 2)
				auto count_size = std::make_unique<unsigned int>(6);
				while(!*it){
					if((it == in.end()) || in.empty()){
						throw std::invalid_argument("Revert of COUNT(X) failed");
					}
					*count_size += 1;
					it++;
					in.pop_front();
				}
				if((in.size() < *count_size) || *count_size > 16){
					throw std::invalid_argument("Revert of COUNT(X) failed");
				}
				auto count_tmp = std::make_unique<unsigned int>(0);
				auto bit_shift = std::make_unique<unsigned int>(0);
				for(auto ite = it + *count_size - 1; ite >= it; ite--, *bit_shift += 1){
					if(*ite){
						*count_tmp |= 1 << *bit_shift;
					}
				}
				*count_tmp += 1;
				it += *count_size;
				pocketplus::utils::pop_n_from_front(in, *count_size);
				for(unsigned int i = 0; i < *count_tmp; i++){
					out.emplace_back(0);
				}
				out.emplace_back(1);
			}
			else{
				throw std::invalid_argument("Revert of COUNT(X) failed");
			}
		}
		else{
			throw std::invalid_argument("Revert of COUNT(X) failed");
		}
	}
}

// Extracts the input vector length from the first compressed frame
unsigned int PocketPlusDecompressor::get_input_vector_length(const std::deque<bool>& input){
	auto input_vector_length = std::make_unique<unsigned int>(0);
	auto input_copy = input;
	// Perform basic plausability check
	if(input_copy.empty() || (input.size() < 7)){
		return *input_vector_length;
	}
	// Initialize bit iterator
	auto bit_position = input_copy.begin();

	// Process first sub vector
	// 5.3.3.1
	std::unique_ptr<bool> d_t;
	std::deque<bool> y_t;

	std::deque<bool> X_t;
	if(!((*bit_position == 1) && (*(bit_position + 1) == 0))){
		undo_rle(input_copy, X_t, bit_position);
	}
	auto X_t_weight = std::make_unique<unsigned int>(hamming_weight(X_t));
	bit_position += 2;
	pocketplus::utils::pop_n_from_front(input_copy, 2);
	// Undo BIT_4(V_t)
	auto robustness_level = std::make_unique<unsigned int>(0);
	for(auto [it, bit_shift] = std::tuple{bit_position + 3, std::make_unique<unsigned int>(0)}; it >= bit_position; it--, *bit_shift += 1){
		if(*it){
			*robustness_level |= 1 << *bit_shift;
		}
	}
	bit_position += 4;
	pocketplus::utils::pop_n_from_front(input_copy, 4);
	
	if((*robustness_level == 0) || (*X_t_weight == 0)){
		// e_t is empty
	}
	else{
		bit_position += 1;
		input_copy.pop_front();
	}
	if((*robustness_level == 0) || (*X_t_weight == 0) || (hamming_weight(y_t) == 0)){
		// k_t is empty
	}
	else{
		for(unsigned int i = 0; i < *X_t_weight; i++){
			bit_position += 1;
			input_copy.pop_front();
		}
		bit_position += 1;
		input_copy.pop_front();
	}
	d_t = std::make_unique<bool>(input_copy.front());
	bit_position += 1;
	input_copy.pop_front();
	if(*d_t == 1){
		return *input_vector_length;
	}

	// Process second sub vector
	// 5.3.3.2
	if(input_copy.front() == 1){
		bit_position++;
		input_copy.pop_front();
		std::deque<bool> mask_mask_shifted;
		if(!((*bit_position == 1) && (*(bit_position + 1) == 0))){
			undo_rle(input_copy, mask_mask_shifted, bit_position);
		}
		bit_position += 2;
		pocketplus::utils::pop_n_from_front(input_copy, 2);
	}
	else{
		bit_position++;
		input_copy.pop_front();
	}

	// Process third sub vector
	// 5.3.3.3
	if(*bit_position){ // rt = 1
		bit_position += 1;
		input_copy.pop_front();
		// Revert COUNT(input_vector_length) operation
		if(*bit_position == 0){
			input_vector_length = std::make_unique<unsigned int>(1);
		}
		else if((*bit_position == 1) && (*(bit_position+1) == 1) && (*(bit_position+2) == 0)){
			bit_position += 3;
			pocketplus::utils::pop_n_from_front(input_copy, 3);
			// Undo BIT_5(A - 2)
			input_vector_length = std::make_unique<unsigned int>(0);
			auto bit_shift = std::make_unique<unsigned int>(0);
			for(auto it = bit_position + 4; it >= bit_position; it--, *bit_shift += 1){
				if(*it){
					*input_vector_length |= 1 << *bit_shift;
				}
			}
			*input_vector_length += 2;
		}
		else if((*bit_position == 1) && (*(bit_position+1) == 1) && (*(bit_position+2) == 1)){
			bit_position += 3;
			pocketplus::utils::pop_n_from_front(input_copy, 3);
			// Undo BIT_E(A - 2)
			auto count_size = std::make_unique<unsigned int>(5);
			while(!*bit_position){
				*count_size += 1;
				bit_position++;
				input_copy.pop_front();
			}
			input_vector_length = std::make_unique<unsigned int>(0);
			auto bit_shift = std::make_unique<unsigned int>(0);
			for(auto ite = bit_position + *count_size; ite >= bit_position; ite--, *bit_shift += 1){
				if(*ite){
					*input_vector_length |= 1 << *bit_shift;
				}
			}
			*input_vector_length += 2;
		}
		else{
			throw std::invalid_argument("Revert of COUNT(input_vector_length) failed");
		}
	}
	//std::cout << "input_vector_length = " << *input_vector_length << std::endl;
	return *input_vector_length;
}

// Tries to decompress a packet from a boolean deque and removes it from the input
std::deque<bool> PocketPlusDecompressor::decompress(std::deque<bool>& input){
	std::deque<bool> output = std::deque<bool>();

	// Perform basic plausability check
	if(input.empty()){
		return output;
	}

	// To check for stuffed zeros the initial and processed length are compared
	input_vector_size_before_processing = std::make_unique<unsigned int>(input.size());

	// Initialize bit iterator
	auto bit_position = input.begin();

	// Check if there are enough bits left for a successful decode
	if(input.size() < *minimum_size){
		input = std::deque<bool>();
		return output;
	}

	//std::cout << "Input:" << std::endl;
	//pocketplus::utils::print_vector(input);

	// Process first sub vector
	// 5.3.3.1
	std::unique_ptr<bool> uncompressed_flag;
	std::unique_ptr<bool> send_mask_flag;
	std::unique_ptr<bool> c_t;
	std::unique_ptr<bool> d_t;
	std::deque<bool> D_t = std::deque<bool>();
	std::deque<bool> k_t = std::deque<bool>();
	std::deque<bool> y_t = std::deque<bool>();

	std::deque<bool> X_t = std::deque<bool>();
	if(!((*bit_position == 1) && (*(bit_position + 1) == 0))){
		undo_rle(input, X_t, bit_position);
	}

	while(X_t.size() < *input_vector_length){
		X_t.emplace_back(0);
	}
	//std::cout << "X_t: ";
	//pocketplus::utils::print_vector(X_t);

	if(X_t.size() == 0){
		D_t.assign(*input_vector_length, 0);
		change_vector.push_back(D_t);
	}
	else{
		D_t = reverse(X_t);
		//std::cout << "D_t: ";
		//pocketplus::utils::print_vector(D_t);
	}
	auto X_t_weight = std::make_unique<unsigned int>(hamming_weight(X_t));
	//std::cout << "X_t_weight: " << *X_t_weight << std::endl;
	bit_position += 2;
	pocketplus::utils::pop_n_from_front(input, 2);
	// Undo BIT_4(V_t)
	robustness_level = std::make_unique<unsigned int>(0);
	auto bit_shift = std::make_unique<unsigned int>(0);
	for(auto it = bit_position + 3; it >= bit_position; it--, *bit_shift += 1){
		if(*it){
			*robustness_level |= 1 << *bit_shift;
		}
	}
	//std::cout << "Input:" << std::endl;
	//pocketplus::utils::print_vector(input);
	//std::cout << "Robustness level: " << *robustness_level << std::endl;
	bit_position += 4;
	pocketplus::utils::pop_n_from_front(input, 4);
	//std::cout << "Input:" << std::endl;
	//pocketplus::utils::print_vector(input);
	std::deque<bool> e_t = std::deque<bool>();
	
	if((*robustness_level == 0) || (*X_t_weight == 0)){
		// e_t is empty
	}
	else{
		if(input.size() == 0){
			throw std::invalid_argument("Invalid input, not enough elements left");
		}
		if(input.front() == 0){
			e_t.emplace_back(0);
			bit_position += 1;
			input.pop_front();
			for(unsigned int i = 0; i < *X_t_weight; i++){
				y_t.emplace_back(0);
			}
		}
		else{
			e_t.emplace_back(1);
			bit_position += 1;
			input.pop_front();
		}
	}
	//std::cout << "y_t.size(): " << y_t.size() << std::endl;
	//pocketplus::utils::print_vector(y_t);
	//if(e_t.size() == 0){
		//std::cout << "e_t: EMPTY" << std::endl;
	//}
	//else{
		//std::cout << "e_t: " << e_t.front() << std::endl;
	//}
	//std::cout << (*robustness_level == 0) << (*X_t_weight == 0) << (hamming_weight(y_t) == 0) << std::endl;
	if(((*robustness_level == 0) || (*X_t_weight == 0) || (hamming_weight(y_t) == 0)) && ((e_t.size() == 0) || (e_t.front() == 0))){
		// k_t is empty
		//std::cout << "must print" << std::endl;
	}
	else{
		// k_t = BE( <~M_t>, X_t )
		//std::cout << "X_t_weight: " << *X_t_weight << std::endl;
		if(input.size() < *X_t_weight){
			throw std::invalid_argument("Invalid input, not enough elements left");
		}
		for(unsigned int i = 0; i < *X_t_weight; i++){
			bit_position += 1;
			k_t.emplace_front(input.front());
			input.pop_front();
		}
		//std::cout << "k_t: " << std::endl;
		//pocketplus::utils::print_vector(k_t);
		if(input.size() == 0){
			throw std::invalid_argument("Invalid input, not enough elements left");
		}
		c_t = std::make_unique<bool>(input.front());
		bit_position += 1;
		input.pop_front();
		//std::cout << "c_t: " << *c_t << std::endl;
	}
	//std::cout << "k_t.size(): " << k_t.size() << std::endl;
	if(input.size() == 0){
		throw std::invalid_argument("Invalid input, not enough elements left");
	}
	d_t = std::make_unique<bool>(input.front());
	bit_position += 1;
	input.pop_front();
	//std::cout << "d_t: " << *d_t << std::endl;
	if(*d_t == 1){
		send_mask_flag = std::make_unique<bool>(0);
		uncompressed_flag = std::make_unique<bool>(0);
	}
	else{
		//uncompressed_flag = std::make_unique<bool>(1);
	}

	// Process second sub vector
	// 5.3.3.2
	//std::cout << "Second vector" << std::endl;
	//std::cout << "Input:" << std::endl;
	//pocketplus::utils::print_vector(input);
	if(*d_t == 1){
		std::deque<bool> M_t = mask_vector.back();
		//build mask from old mask and mask changes D_t with values k_t inserted
		auto it_D_t = D_t.begin();
		auto it_M_t = M_t.begin();
		auto it_old_mask = mask_vector.back().begin();
		//std::cout << "k_t: " << std::endl;
		//pocketplus::utils::print_vector(k_t);
		//std::cout << "y_t: " << std::endl;
		//pocketplus::utils::print_vector(y_t);
		if(y_t.size() > 0){
			auto it_y_t = y_t.rbegin();
			while(it_M_t != M_t.end()){
				if(*it_D_t){
					if(it_y_t == y_t.rend()){
						throw std::invalid_argument("Invalid input, not enough y_t bits left");
					}
					*it_M_t = !*it_y_t; // Undo ~M_t
					it_y_t++;
				}
				it_D_t++;
				it_M_t++;
				it_old_mask++;
			}
		}
		else if(k_t.size() > 0){
			auto it_k_t = k_t.rbegin();
			while(it_M_t != M_t.end()){
				if(*it_D_t){
					*it_M_t = !*it_k_t; // Undo ~M_t
					it_k_t++;
				}
				it_D_t++;
				it_M_t++;
				it_old_mask++;
			}
		}
		else{ // R_t = 0
			while(it_M_t != M_t.end()){
				*it_M_t = *it_M_t ^ *it_D_t;
				it_M_t++;
				it_D_t++;
			}
		}
		//std::cout << "old_mask: " << std::endl;
		//pocketplus::utils::print_vector(mask_vector.back());
		mask_vector.push_back(M_t);
		//std::cout << "D_t: " << std::endl;
		//pocketplus::utils::print_vector(D_t);
		//std::cout << "M_t: " << std::endl;
		//pocketplus::utils::print_vector(M_t);
	}
	else{
		if(input.size() == 0){
			throw std::invalid_argument("Invalid input, not enough elements left");
		}
		if(input.front() == 1){ // send_mask_flag (f_t) = 1
			send_mask_flag = std::make_unique<bool>(1);
			//std::cout << "send_mask_flag (f_t) = 1" << std::endl;
			// '1' | RLE(<(M_t XOR (M_t<<1)))>) | '10'
			bit_position += 1;
			input.pop_front();
			std::deque<bool> mask_mask_shifted = std::deque<bool>();
			if(!((*bit_position == 1) && (*(bit_position + 1) == 0))){
				undo_rle(input, mask_mask_shifted, bit_position);
				if(mask_mask_shifted.size() > *input_vector_length){
					throw std::invalid_argument("Invalid shifted mask received, too large to fit input vector length");
				}
				while(mask_mask_shifted.size() < *input_vector_length){
					mask_mask_shifted.emplace_back(0);
				}
				std::deque<bool> M_t = std::deque<bool>();
				auto it = mask_mask_shifted.begin();
				if(*it == 1){
					M_t.emplace_front(1);
				}
				else{
					M_t.emplace_front(0);
				}
				it++;
				for(; it != mask_mask_shifted.end(); it++){
					if((*it == 1) && (M_t.front() == 0)){
						M_t.emplace_front(1);
					}
					else if((*it == 0) && (M_t.front() == 1)){	
						M_t.emplace_front(1);
					}
					else{
						M_t.emplace_front(0);
					}
				}
				while(M_t.size() < *input_vector_length){
					M_t.emplace_front(0);
				}
				// Insert mask changes into the decoded mask
				if(y_t.size() > 0){
					auto it_D_t = D_t.begin();
					auto it_M_t_new = M_t.begin();
					auto it_old_mask = mask_vector.back().begin();
					auto it_y_t = y_t.rbegin();
					while(it_M_t_new != M_t.end()){
						if(*it_D_t){
							*it_M_t_new = !*it_y_t; // Undo ~M_t
							it_y_t++;
						}
						it_D_t++;
						it_M_t_new++;
						it_old_mask++;
					}
				}
				else if(k_t.size() > 0){
					auto it_D_t = D_t.begin();
					auto it_M_t_new = M_t.begin();
					auto it_old_mask = mask_vector.back().begin();
					auto it_k_t = k_t.rbegin();
					while(it_M_t_new != M_t.end()){
						if(*it_D_t){
							*it_M_t_new = !*it_k_t; // Undo ~M_t
							it_k_t++;
						}
						it_D_t++;
						it_M_t_new++;
						it_old_mask++;
					}
				}
				if(mask_vector.back().size() != *input_vector_length){
					throw std::invalid_argument("Invalid input, mask size is not the input vector length");
				}
				mask_vector.push_back(M_t);
				//std::cout << "M_t:" << std::endl;
				//pocketplus::utils::print_vector(M_t);
			}
			else{ // RLE(<(M_t XOR M_t<<))>) == NULL
				std::deque<bool> M_t = std::deque<bool>();
				M_t.assign(*input_vector_length, 0);
				mask_vector.push_back(M_t);
			}
			bit_position += 2;
			pocketplus::utils::pop_n_from_front(input, 2);
		}
		else{ // otherwise
			send_mask_flag = std::make_unique<bool>(0);
			uncompressed_flag = std::make_unique<bool>(1);
			//std::cout << "send_mask_flag (f_t) = 0" << std::endl;
			bit_position++;
			input.pop_front();
			std::deque<bool> M_t = mask_vector.back();
			if(M_t.size() != D_t.size()){
				throw std::invalid_argument("Invalid packet received, D_t size does not match M_t size");
			}
			auto it_M_t = M_t.rbegin();
			auto it_D_t = D_t.rbegin();
			auto it_y_t = y_t.begin();
			if(y_t.size() != hamming_weight(D_t)){
				throw std::invalid_argument("Invalid packet received, y_t size does not match hamming weight of D_t");
			}
			while(it_D_t != D_t.rend()){
				if(*it_D_t){
					*it_M_t = *it_M_t ^ (!*it_y_t);
					it_y_t++;
				}
				it_D_t++;
				it_M_t++;
			}
			mask_vector.push_back(M_t);
			//std::cout << "M_t: " << std::endl;
			//pocketplus::utils::print_vector(M_t);
		}
	}

	// Process third sub vector
	// 5.3.3.3
	//std::cout << "Third vector" << std::endl;
	//std::cout << "Input:" << std::endl;
	//pocketplus::utils::print_vector(input);
	if(*d_t == 1){ // d_t = 1
		if(*t == 0){
			throw std::invalid_argument("Invalid third subvector for the first packet, d_t = 1 is not allowed here");
		}
		if(c_t){
			if(*c_t == 1){ // c_t = 1
				//std::cout << "Case 1" << std::endl;
				//std::cout << "X_t: " << std::endl;
				//pocketplus::utils::print_vector(X_t);
				//std::cout << "M_t: " << std::endl;
				//pocketplus::utils::print_vector(mask_vector.back());
				// BE(I_t, (X_t OR M_t))
				if(mask_vector.back().size() != *input_vector_length){
					throw std::invalid_argument("Invalid input, mask size is not the input vector length");
				}
				output.assign(*input_vector_length, 0); // Fill the output vector with zeros
				auto it_mask = mask_vector.back().rbegin();
				auto it_X_t = X_t.begin();
				auto it_output = output.rbegin();
				auto it_last_input = input_vector.back().rbegin();
				for(; it_mask != mask_vector.back().rend();){
					if((*it_mask || *it_X_t) == 1){
						if(input.size() == 0){
							throw std::invalid_argument("Invalid input, not enough elements left");
						}
						*it_output = input.front();
						input.pop_front();
					}
					else{
						*it_output = *it_last_input;
					}
					it_mask++;
					it_X_t++;
					it_output++;
					it_last_input++;
				}
				input_vector.push_back(output);
			}
			else{ // c_t /= 1
				// BE(I_t, M_t) values of I_t at the positions where M_t is one
				// Evaluate mask and add bits in predictable positions from previous input_vector
				output.assign(*input_vector_length, 0); // Fill the output vector with zeros
				auto it_mask = mask_vector.back().rbegin();
				auto it_output = output.rbegin();
				auto it_last_input = input_vector.back().rbegin();
				for(; it_mask != mask_vector.back().rend();){
					if(*it_mask == 1){
						if(input.size() == 0){
							throw std::invalid_argument("Invalid input, not enough elements left");
						}
						*it_output = input.front();
						input.pop_front();
					}
					else{
						*it_output = *it_last_input;
					}
					it_mask++;
					it_output++;
					it_last_input++;
				}
				input_vector.push_back(output);
			}
		}
		else{ // c_t /= 1
			// BE(I_t, M_t) values of I_t at the positions where M_t is one
			// Evaluate mask and add bits in predictable positions from previous input_vector
			output.assign(*input_vector_length, 0); // Fill the output vector with zeros
			auto it_mask = mask_vector.back().rbegin();
			auto it_output = output.rbegin();
			auto it_last_input = input_vector.back().rbegin();
			for(; it_mask != mask_vector.back().rend();){
				if(*it_mask == 1){
					if(input.size() == 0){
						throw std::invalid_argument("Invalid input, not enough elements left");
					}
					*it_output = input.front();
					input.pop_front();
				}
				else{
					*it_output = *it_last_input;
				}
				it_mask++;
				it_output++;
				it_last_input++;
			}
			input_vector.push_back(output);
		}
	}
	else if(*bit_position){ // rt = 1
		uncompressed_flag = std::make_unique<bool>(1);
		if(input.size() == 0){
			throw std::invalid_argument("Invalid input, not enough elements left");
		}
		bit_position += 1;
		input.pop_front();
		// Revert COUNT(input_vector_length) operation
		std::unique_ptr<unsigned int> tmp_input_vector_length;
		if(*bit_position == 0){
			tmp_input_vector_length = std::make_unique<unsigned int>(1);
			if(input.size() == 0){
				throw std::invalid_argument("Invalid input, not enough elements left");
			}
			bit_position += 1;
			pocketplus::utils::pop_n_from_front(input, 1);
			if(*input_vector_length != *tmp_input_vector_length){
				throw std::invalid_argument("Input vector length of this packet does not match the previously set length");
			}
		}
		else if((*bit_position == 1) && (*(bit_position+1) == 1) && (*(bit_position+2) == 0)){
			if(input.size() < 8){
				throw std::invalid_argument("Invalid input, not enough elements left");
			}
			bit_position += 3;
			pocketplus::utils::pop_n_from_front(input, 3);
			// Undo BIT_5(A - 2)
			tmp_input_vector_length = std::make_unique<unsigned int>(0);
			for(auto [it, bit_shift] = std::tuple{bit_position + 4, std::make_unique<unsigned int>(0)}; it >= bit_position; it--, *bit_shift += 1){
				if(*it){
					*tmp_input_vector_length |= 1 << *bit_shift;
				}
			}
			*tmp_input_vector_length += 2;
			bit_position += 5;
			pocketplus::utils::pop_n_from_front(input, 5);
			if(*input_vector_length != *tmp_input_vector_length){
				throw std::invalid_argument("Input vector length of this packet does not match the previously set length");
			}
			//std::cout << "input_vector_length=" << *input_vector_length << std::endl;
		}
		else if((*bit_position == 1) && (*(bit_position+1) == 1) && (*(bit_position+2) == 1)){
			if(input.size() < 3){
				throw std::invalid_argument("Invalid input, not enough elements left");
			}
			bit_position += 3;
			pocketplus::utils::pop_n_from_front(input, 3);
			// Undo BIT_E(A - 2)
			auto count_size = std::make_unique<unsigned int>(5);
			while(!*bit_position){
				*count_size += 1;
				if(input.size() == 0){
					throw std::invalid_argument("Invalid input, not enough elements left");
				}
				bit_position++;
				input.pop_front();
			}
			tmp_input_vector_length = std::make_unique<unsigned int>(0);
			for(auto [it, bit_shift] = std::tuple{bit_position + *count_size, std::make_unique<unsigned int>(0)}; it >= bit_position; it--, *bit_shift += 1){
				if(*it){
					*tmp_input_vector_length |= 1 << *bit_shift;
				}
			}
			*tmp_input_vector_length += 2;
			bit_position += *count_size + 1;
			if(input.size() < *count_size + 1){
				throw std::invalid_argument("Invalid input, not enough elements left");
			}
			pocketplus::utils::pop_n_from_front(input, *count_size + 1);
			if(*input_vector_length != *tmp_input_vector_length){
				throw std::invalid_argument("Input vector length of this packet does not match the previously set length");
			}
			//std::cout << "input_vector_length=" << *input_vector_length << std::endl;
		}
		else{
			throw std::invalid_argument("Revert of COUNT(input_vector_length) failed");
		}
		if(input.size() >= *input_vector_length){
			output.insert(output.end(), std::make_move_iterator(input.begin()), std::make_move_iterator(input.begin() + *input_vector_length));
			input.erase(input.begin(), input.begin() + *input_vector_length );
			input_vector.push_back(output);
		}
		else{
			throw std::invalid_argument("Not enough bits left to extract data frame of length " + std::to_string(*input_vector_length) + " and n_t");
		}
	}
	else{
		if(*t == 0){
			throw std::invalid_argument("Invalid third subvector for the first packet, must be uncompressed");
		}
		if(input.size() == 0){
			throw std::invalid_argument("Invalid input, not enough elements left");
		}
		bit_position += 1;
		input.pop_front();
		uncompressed_flag = std::make_unique<bool>(0);
		if(uncompressed_flag && send_mask_flag && c_t){
			if((*uncompressed_flag == 0) && (*send_mask_flag == 1) && (*c_t == 1)){
				// BE(I_t, (X_t OR M_t))
				output.assign(*input_vector_length, 0); // Fill the output vector with zeros
				auto it_mask = mask_vector.back().rbegin();
				auto it_X_t = X_t.begin();
				auto it_output = output.rbegin();
				auto it_last_input = input_vector.back().rbegin();
				for(; it_mask != mask_vector.back().rend();){
					if((*it_mask || *it_X_t) == 1){
						if(input.size() == 0){
							throw std::invalid_argument("Invalid input, not enough elements left");
						}
						*it_output = input.front();
						input.pop_front();
					}
					else{
						*it_output = *it_last_input;
					}
					it_mask++;
					it_X_t++;
					it_output++;
					it_last_input++;
				}
				input_vector.push_back(output);
			}
			else{
				// BE(I_t, M_t) values of I_t at the positions where M_t is one
				// Evaluate mask and add bits in predictable positions from previous input_vector
				output.assign(*input_vector_length, 0); // Fill the output vector with zeros
				auto it_mask = mask_vector.back().rbegin();
				auto it_output = output.rbegin();
				auto it_last_input = input_vector.back().rbegin();
				for(; it_mask != mask_vector.back().rend();){
					if(*it_mask == 1){
						if(input.size() == 0){
							throw std::invalid_argument("Invalid input, not enough elements left");
						}
						*it_output = input.front();
						input.pop_front();
					}
					else{
						*it_output = *it_last_input;
					}
					it_mask++;
					it_output++;
					it_last_input++;
				}
				input_vector.push_back(output);
			}
		}
		else{
			// BE(I_t, M_t) values of I_t at the positions where M_t is one
			// Evaluate mask and add bits in predictable positions from previous input_vector
			output.assign(*input_vector_length, 0); // Fill the output vector with zeros
			auto it_mask = mask_vector.back().rbegin();
			auto it_output = output.rbegin();
			auto it_last_input = input_vector.back().rbegin();
			for(; it_mask != mask_vector.back().rend();){
				if(*it_mask == 1){
					if(input.size() == 0){
						throw std::invalid_argument("Invalid input, not enough elements left");
					}
					*it_output = input.front();
					input.pop_front();
				}
				else{
					*it_output = *it_last_input;
				}
				it_mask++;
				it_output++;
				it_last_input++;
			}
			input_vector.push_back(output);
		}
	}
	//std::cout << "Extracted:" << std::endl;
	//pocketplus::utils::print_vector(output);
	*t += 1;
	if(!input.empty()){
		pocketplus::utils::pop_n_from_front(input, (8 - ((*input_vector_size_before_processing - input.size()) % 8)) % 8);
	}
	//std::cout << "INPUT remaining:" << std::endl;
	//pocketplus::utils::print_vector(input);
	return output;
}