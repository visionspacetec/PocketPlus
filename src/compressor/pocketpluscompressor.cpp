#include "pocketpluscompressor.h"

using namespace pocketplus::compressor;

void PocketPlusCompressor::set_input_vector_length(const unsigned int& vector_length){
	if(input_vector_length){
		throw std::invalid_argument("Input vector length alrady set");
	}
	if ((vector_length < 1) || (vector_length > 65535)){ 
		throw std::out_of_range("1 <= input_vector_length <= 2^16-1 (65535");
	}
	input_vector_length = std::make_shared<unsigned int>(vector_length); // F // User defined value
	input_vector_length_count = count(*input_vector_length);
	initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0 // ############ ToDo: Make initial mask user defined
	mask_new = initial_mask_vector;
	input_old.assign(*input_vector_length, 0);
	mask_old.assign(*input_vector_length, 0); // M_t
	mask_build_old.assign(*input_vector_length, 0); // B_t // B_0 = 0
	mask_build_new.assign(*input_vector_length, 0); // B_t // B_0 = 0
	mask_change_0.assign(*input_vector_length, 0); 
}

unsigned int PocketPlusCompressor::get_input_vector_length(){
	if(input_vector_length){
		return *input_vector_length;
	}
	else{
		return 0;
	}
}

// 5.3.1.1 Counter encoding function
std::deque<bool> PocketPlusCompressor::count(const unsigned int& a){
	std::deque<bool> output_vector;
	if((a == 0) || (a > 65535)){
		throw std::out_of_range("1 <= a <= 2^16 - 1 = 65535");
	}
	else if(a == 1){
		output_vector.insert(output_vector.begin(), {0});
	}
	else if ((2 <= a) && (a <= 33)){
		auto length = std::make_unique<unsigned int>(5);
		for(unsigned int i = 0; i < *length; i++){
			output_vector.emplace_front(((a - 2) >> i) & 1);
		}
		output_vector.insert(output_vector.begin(), {1, 1, 0});
	}
	else{
		auto length = std::make_unique<unsigned int>((2 * std::floor(std::log2(a - 2) + 1)) - 6);
		for(unsigned int i = 0; i < *length; i++){
			output_vector.emplace_front(((a - 2) >> i) & 1);
		}
		output_vector.insert(output_vector.begin(), {1, 1, 1});
	}
	return output_vector;
}

// 5.3.1.2 Run length encoding
std::deque<bool> PocketPlusCompressor::run_length_encoding(const std::deque<bool>& a){
	std::deque<bool> output_vector;
	std::deque<bool> count_value;
	auto zero_counter = std::make_unique<unsigned int>(0);
	for(auto i = a.begin(); i != a.end(); i++){
		*zero_counter += 1;
		if(!(*i == 0)){
			count_value = count(*zero_counter);
			output_vector.insert(output_vector.end(), count_value.begin(), count_value.end());
			*zero_counter = 0;
		}
	}
	return output_vector;
}

// 5.3.1.3 Bit extraction function
std::deque<bool> PocketPlusCompressor::bit_extraction(const std::deque<bool>& a, const std::deque<bool>& b){
	if(!(a.size() == b.size())){
		throw std::invalid_argument("a.size() == b.size()");
	}
	std::deque<bool> output_vector;
	for(size_t i = 0; i < a.size(); i++){
		if(b.at(i) == 1){
			output_vector.emplace_back(a.at(i));
		}
	}
	return output_vector;
}

// Hamming weight - Number of ones in vector
unsigned int PocketPlusCompressor::hamming_weight(const std::deque<bool>& a){
	unsigned int output = 0;
	for(size_t i = 0; i < a.size(); i++){
		output += a.at(i);
	}
	return output;
}

// Reverse deque helper function, denoted < A > in the standard
std::deque<bool> PocketPlusCompressor::reverse(const std::deque<bool>& a){
	std::deque<bool> output_vector;
	for(auto i = a.rbegin(); i != a.rend(); i++ ){
		output_vector.emplace_back(*i);
	}
	return output_vector;
}

// Inverse deque helper function, denoted ~A in the standard
std::deque<bool> PocketPlusCompressor::inverse(const std::deque<bool>& a){
	std::deque<bool> output_vector;
	for(auto i = a.begin(); i != a.end(); i++ ){
		output_vector.emplace_back(!*i);
	}
	return output_vector;
}

// Performs the actual compression
std::deque<bool> PocketPlusCompressor::compress(
	const std::deque<bool>& input_new, 
	const unsigned int& robustness_level, // R_t // User defined value
	const bool& new_mask_flag, // p_t
	const bool& send_mask_flag, // f_t
	const bool& uncompressed_flag // r_t
	){
	if(!input_vector_length){
		throw std::invalid_argument("Input vector length must be set before calling compress");
	}
	// Section 3.3.2 Compressor parameters
	if(*input_vector_length != input_new.size()){
		throw std::invalid_argument("Input vector must have the predefined input_vector_length");
	}
	if ((robustness_level < *robustness_level_min) || (robustness_level > *robustness_level_max)){
		throw std::out_of_range("0 <= robustness_level <= 7");
	}
	if(*t <= robustness_level){
		if(!send_mask_flag){
			throw std::invalid_argument("For t <= robustness level, send mask flag must be true");
		}
		if(!uncompressed_flag){
			throw std::invalid_argument("For t <= robustness level, uncompressed flag must be true");
		}
	}
	mask_flag.emplace_back(new_mask_flag);
	auto it_mask_flag = mask_flag.rbegin();
	auto mask_flag_weight = std::make_unique<unsigned int>(0);
	auto robustness_level_counter = std::make_unique<unsigned int>(0);
	while((it_mask_flag != mask_flag.rend()) && (*robustness_level_counter <= robustness_level)){
		*mask_flag_weight += *it_mask_flag;
		it_mask_flag++;
		*robustness_level_counter += 1;
	}
	if(*mask_flag_weight > 1){
		throw std::invalid_argument("New mask flag must be false due to recent mask update");
	}
	if(mask_flag.size() > 7){
		mask_flag.pop_front();
	}
	first_binary_vector.clear();
	second_binary_vector.clear();
	third_binary_vector.clear();
	output.clear();

	// Section 4.2 Mask update    
	// Section 4.2.1
	if(!(*t == 0) && !mask_flag.back()){
		std::generate(
			mask_build_new.begin(), 
			mask_build_new.end(), 
			[mo=mask_build_old, input_new, io=input_old, n = 0]() mutable {
				auto out = (!input_new.at(n) != !io.at(n)) || mo.at(n);
				n++;
				return out;
			}
		);
	}
	else{
		std::fill(mask_build_new.begin(), mask_build_new.end(), 0);
	}

	// Section 4.2.2
	while(mask_change_vector.size() > *robustness_level_max + 1){
		mask_change_vector.pop_front();
	}
	mask_change_vector.emplace_front(mask_change_0);

	if(!(*t == 0)){
		if(!mask_flag.back()){
			std::generate(
				mask_new.begin(), 
				mask_new.end(), 
				[mo=mask_old, input_new, io=input_old, n = 0]() mutable {
					auto out = (!input_new.at(n) != !io.at(n)) || mo.at(n);
					n++;
					return out;
				}
			);
		}
		else{
			std::generate(
				mask_new.begin(), 
				mask_new.end(), 
				[mbo=mask_build_old, input_new, io=input_old, n = 0]() mutable {
					auto out = (!input_new.at(n) != !io.at(n)) || mbo.at(n);
					n++;
					return out;
				}
			);
		}
		// Section 4.2.3
		std::generate(
			mask_change_vector.at(0).begin(), 
			mask_change_vector.at(0).end(), 
			[mn=mask_new, mo=mask_old, n = 0]() mutable {
				auto out = !mn.at(n) != !mo.at(n);
				n++;
				return out;
			}
		);
	}
	
	// 5.3.2 Encoding step
	// 5.3.2.1
	std::unique_ptr<bool> d_t;
	if(!send_mask_flag && !uncompressed_flag){
		d_t = std::make_unique<bool>(1);
	}
	else{
		d_t = std::make_unique<bool>(0);
	}

	// 5.3.2.2 first binary vector
	// Equation (15)
	X_t.resize(*input_vector_length);
	if(robustness_level == 0){
		// X_t = <D_t>
		X_t = reverse(mask_change_vector.at(0));
	}
	else if((int)(*t - robustness_level) <= 0){
		// X_t = [<D_1 OR D_2 OR ... OR D_t>]
		X_t.assign(*input_vector_length, 0);
		for(unsigned int i = 0; i < mask_change_vector.size(); i++){
			for(unsigned int index = 0; index < mask_change_vector.at(i).size(); index++){
				X_t.at(index) = mask_change_vector.at(i).at(index) || X_t.at(index);
			}
		}
		X_t = reverse(X_t); // ToDo: Just go the other way thru the for loop to avoid this step
	}
	else{
		// X_t = [<D_(t-robustness_level) OR D_(t-robustness_level)+1 OR ... OR D_t>]
		X_t.assign(*input_vector_length, 0); 
		for(unsigned int i = 0; i < robustness_level + 1; i++){
			for(unsigned int index = 0; index < mask_change_vector.at(i).size(); index++){
				X_t.at(index) = mask_change_vector.at(i).at(index) || X_t.at(index);
			}
		}
		X_t = reverse(X_t); // ToDo: Just go the other way thru the for loop to avoid this step
		//std::cout << "X_t case 3" << std::endl;
	}
	// Equation (16)
	y_t = bit_extraction(reverse(inverse(mask_new)), X_t);
	//std::cout<< "y_t" << std::endl;
	//pocketplus::utils::print_vector(y_t);
	// Equation (17)
	e_t.clear();
	if((robustness_level == 0) || (hamming_weight(X_t) == 0)){}
	else if(((hamming_weight(y_t) == 0) && (robustness_level > 0)) && (hamming_weight(X_t) != 0)){
		e_t = {0};
	}
	else{
		e_t = {1};
	}
	// Equation (18)
	if((robustness_level == 0) || (hamming_weight(X_t) == 0) || (hamming_weight(y_t) == 0)){
		// k_t = empty
	}
	else{
		k_t = bit_extraction(inverse(mask_new), reverse(X_t));
	}
	// Equation (14)
	X_t_rle = run_length_encoding(X_t);
	robustness_level_bit_3.clear();
	for(unsigned int i = 0; i < 3; i++){
		robustness_level_bit_3.emplace_front((robustness_level >> i) & 1);
	}
	first_binary_vector.clear();
	first_binary_vector.insert(first_binary_vector.end(), X_t_rle.begin(), X_t_rle.end());
	first_binary_vector.insert(first_binary_vector.end(), {1, 0});
	first_binary_vector.insert(first_binary_vector.end(), robustness_level_bit_3.begin(), robustness_level_bit_3.end());
	first_binary_vector.insert(first_binary_vector.end(), e_t.begin(), e_t.end());
	first_binary_vector.insert(first_binary_vector.end(), k_t.begin(), k_t.end());
	first_binary_vector.emplace_back(*d_t);

	// 5.3.2.3 Second binary vector
	// Equation (19)
	second_binary_vector.clear();
	if(*d_t == 1){
		// Empty second binary vector
	}
	else if(send_mask_flag == 1){
		mask_shifted_rle = mask_new;
		mask_shifted_rle.pop_front();
		mask_shifted_rle.emplace_back(0);
		std::generate(
			mask_shifted_rle.begin(), 
			mask_shifted_rle.end(), 
			[mn=mask_new, msr=mask_shifted_rle, n = 0]() mutable {
				auto out = mn.at(n) ^ msr.at(n);
				n++;
				return out;
			}
		);
		mask_shifted_rle = reverse(mask_shifted_rle);
		mask_shifted_rle = run_length_encoding(mask_shifted_rle);
		second_binary_vector.emplace_back(1);
		second_binary_vector.insert(second_binary_vector.end(), mask_shifted_rle.begin(), mask_shifted_rle.end());
		second_binary_vector.insert(second_binary_vector.end(), {1, 0});
	}
	else{
		second_binary_vector.emplace_back(0);
	}

	// 5.3.2.4 Third binary vector
	// Equation (20)
	third_binary_vector.clear();
	if(*d_t == 1){
		input_mask_bit_extraction = bit_extraction(input_new, mask_new);
		third_binary_vector.insert(third_binary_vector.end(), input_mask_bit_extraction.begin(), input_mask_bit_extraction.end());
	}
	else if(uncompressed_flag){
		third_binary_vector.emplace_back(1);
		third_binary_vector.insert(third_binary_vector.end(), input_vector_length_count.begin(), input_vector_length_count.end());
		third_binary_vector.insert(third_binary_vector.end(), input_new.begin(), input_new.end());
	}
	else{
		third_binary_vector.emplace_back(0);
		input_mask_bit_extraction = bit_extraction(input_new, mask_new);
		third_binary_vector.insert(third_binary_vector.end(), input_mask_bit_extraction.begin(), input_mask_bit_extraction.end());
	}

	// Output vector
	// Equation (9)
	output.insert(output.end(), first_binary_vector.begin(), first_binary_vector.end());
	output.insert(output.end(), second_binary_vector.begin(), second_binary_vector.end());
	output.insert(output.end(), third_binary_vector.begin(), third_binary_vector.end());

	*t = *t + 1;
	input_old = input_new;
	mask_old = mask_new;
	mask_build_old = mask_build_new;

	//std::cout << "First:" << std::endl;
	//pocketplus::utils::print_vector(first_binary_vector);
	//std::cout << "Second:" << std::endl;
	//pocketplus::utils::print_vector(second_binary_vector);
	//std::cout << "Third:" << std::endl;
	//pocketplus::utils::print_vector(third_binary_vector);

	return output;
}