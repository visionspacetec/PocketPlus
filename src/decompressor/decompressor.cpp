#include "decompressor.h"

using namespace pocketplus::decompressor;

std::deque<bool> decompress(std::deque<bool>& input){
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
            return output;
        }
    }

    

    return output;
}