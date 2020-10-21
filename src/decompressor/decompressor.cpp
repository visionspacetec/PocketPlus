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
    // 5.3.2.2
    if(hamming_weight_in_range(bit_position, bit_position + 6) == 1){ // nt = 0
        std::cout << "nt = 0" << std::endl;
        bit_position += 5;
    }
    else{
        std::cout << "nt != 0" << std::endl;
    }

    return output;
}