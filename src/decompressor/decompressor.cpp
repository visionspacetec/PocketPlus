#include "decompressor.h"

using namespace pocketplus::decompressor;

// Hamming weight in range - Number of ones in boolean deque in the given range
unsigned int PocketPlusDecompressor::hamming_weight_in_range(std::deque<bool>::iterator start, std::deque<bool>::iterator stop){
    unsigned int output = 0;
    for(auto i = start; i < stop; i++){
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
        input.pop_back();
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
    if(hamming_weight_in_range(bit_position, bit_position + 6) == 1){ // nt = 0
        std::cout << "send_changes_flag (n_t) = 0" << std::endl;
        send_changes_flag = std::make_unique<bool>(0);
        std::cout << "uncompressed_flag (r_t) = 1" << std::endl;
        uncompressed_flag = std::make_unique<bool>(1); // From the standard: if nt = 0 -> rt = 1
        bit_position += 5;
    }
    else{
        send_changes_flag = std::make_unique<bool>(1);
        std::cout << "send_changes_flag (n_t) = 0" << std::endl;
        // ToDo
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
    if(*d_t == 1){ // if dt = 1
        // Do nothing
    }
    else if(*send_mask_flag == 1){ // if ft = 1
        // ToDo
    }
    else{ // otherwise
        // ToDo
    }

    // Process third sub vector
    // 5.3.2.4
    if(*d_t == 1){ // d_t = 1
        //ToDo
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 1)){ // rt = 1 and vt = 1
        //ToDo
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 0)){ // rt = 1 and vt = 0
        //ToDo
    }
    else{
        //ToDo
    }

    return output;
}