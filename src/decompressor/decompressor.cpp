#include "decompressor.h"

using namespace pocketplus::decompressor;

// Hamming weight in range - Number of ones in boolean deque in the given range
unsigned int PocketPlusDecompressor::hamming_weight_in_range(std::deque<bool>::iterator start, std::deque<bool>::iterator stop){
    unsigned int output = 0;
    for(auto i = start; i <= stop; i++){
        output += *i;
    }
    return output;
}

// Tries to decompress a compress a packet from a boolean deque and removes it from the input
std::deque<bool> PocketPlusDecompressor::decompress(std::deque<bool>& input){
    std::deque<bool> output;

    // Perform basic plausability check
    if(input.empty()){
        return output;
    }

    // Initialize bit iterator
    auto bit_position = input.begin();

    // Jump over leading zeros
    while(*bit_position == 0){
        bit_position++;
        input.pop_front();
        if(bit_position == input.end()){
            input.clear();
            return output;
        }
    }

    // Check if there are enough bits left for a successful decode
    if(input.size() < *minimum_size){
        input.clear();
        return output;
    }

    for(auto i = bit_position; i < bit_position + 6; i++){
        std::cout << *i << " ";
    }
    std::cout << std::endl;

    // Process first sub vector
    // 5.3.2.2
    std::unique_ptr<bool> send_changes_flag;
    std::unique_ptr<bool> uncompressed_flag;
    if(hamming_weight_in_range(bit_position, bit_position + 5) == 1){ // nt = 0
        std::cout << "send_changes_flag (n_t) = 0" << std::endl;
        send_changes_flag = std::make_unique<bool>(0);
        std::cout << "uncompressed_flag (r_t) = 1" << std::endl;
        uncompressed_flag = std::make_unique<bool>(1); // From the standard: if nt = 0 -> rt = 1
        bit_position += 6;
        for(auto i = 0; i < 6; i++){
            input.pop_front();
        }
    }
    else{
        send_changes_flag = std::make_unique<bool>(1);
        std::cout << "send_changes_flag (n_t) = 1" << std::endl;
        std::deque<bool> X_t;
        auto done = std::make_unique<bool>(0);
        while(!*done){
            // D_t = M_t XOR M_t-1 (mask change vector)
            // X_t = < D_t >
            // What is the minumum size that RLE(X_t) can have? --> NULL!
            if(*bit_position == 0){

            }
        }
        // ############ ToDo!!!!!!!!!!!!!!!!!
    }

    // Process second sub vector
    std::unique_ptr<bool> d_t;
    std::unique_ptr<bool> send_input_length_flag;
    std::unique_ptr<bool> send_mask_flag;
    if(*t == 0){
        std::cout << "d_t = 0" << std::endl;
        d_t = std::make_unique<bool>(0);
        std::cout << "send_input_length_flag (v_t) = 1" << std::endl;
        send_input_length_flag = std::make_unique<bool>(1);
        std::cout << "send_mask_flag (f_t) = 0" << std::endl;
        send_mask_flag = std::make_unique<bool>(0);
    }

    // 5.3.2.3
    pocketplus::utils::print_vector(input);
    if(*d_t == 1){ // if dt = 1
        // Do nothing
    }
    else if(*send_mask_flag == 1){ // if ft = 1
        // ToDo
    }
    else{ // otherwise
        // ToDo
        bit_position++;
        input.pop_front();
    }
    pocketplus::utils::print_vector(input);

    // Process third sub vector
    // 5.3.2.4
    if(*d_t == 1){ // d_t = 1
        //ToDo
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 1)){ // rt = 1 and vt = 1
        //ToDo
        if(hamming_weight_in_range(bit_position, bit_position + 1) == 2){
            bit_position += 2;
            pocketplus::utils::pop_n_from_front(input, 2);
        }
        else{
            throw std::invalid_argument("Something went wrong");
        }
        // Revert COUNT(input_vector_length) operation
        if(*bit_position == 0){
            input_vector_length = std::make_unique<unsigned int>(1);
        }
        else if(hamming_weight_in_range(bit_position, bit_position + 2) == 2){
            std::cout << "2<=input_vector_length<=33" << std::endl;
            bit_position += 3;
            pocketplus::utils::pop_n_from_front(input, 3);
            // Undo BIT_5(A - 2)
            input_vector_length = std::make_unique<unsigned int>(0);
            auto bit_shift = std::make_unique<unsigned int>(0);
            for(auto it = bit_position + 4; it >= bit_position; it--, *bit_shift += 1){
                if(*it){
                    *input_vector_length |= 1 << *bit_shift;
                }
            }
            *input_vector_length += 2;
            bit_position += 5;
            pocketplus::utils::pop_n_from_front(input, 5);
            std::cout << "input_vector_length=" << *input_vector_length << std::endl;
        }
        else if(hamming_weight_in_range(bit_position, bit_position + 2) == 3){
            std::cout << "input_vector_length>=34" << std::endl;
            // ToDo
        }
        else{
            throw std::invalid_argument("Revert of COUNT(input_vector_length) failed");
        }
        pocketplus::utils::print_vector(input);
        if(input.size() >= *input_vector_length){
            output.insert(output.end(), std::make_move_iterator(input.begin()), std::make_move_iterator(input.begin() + *input_vector_length));
            std::cout << "Extracted:" << std::endl;
            pocketplus::utils::print_vector(output);
            input.erase(input.begin(), input.begin() + *input_vector_length );
        }
        else{
            throw std::invalid_argument("Not enough bits left to extract data frame of length " + std::to_string(*input_vector_length) + " and n_t");
        }
        *send_changes_flag = input.front();
        input.pop_front();
        std::cout << "send_changes_flag (n_t) = " << *send_changes_flag << std::endl;
        *t += 1;
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 0)){ // rt = 1 and vt = 0
        //ToDo
    }
    else{
        //ToDo
    }
    std::cout << "INPUT remaining:" << std::endl;
    pocketplus::utils::print_vector(input);
    return output;
}