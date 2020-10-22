#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <deque>
#include <memory>
#include <numeric>

#include "../utils/utils.h"

namespace pocketplus {
namespace decompressor {

class PocketPlusDecompressor{
    // Private functions
    // Hamming weight - Number of ones in vector
    unsigned int hamming_weight_in_range(std::deque<bool>::iterator start, std::deque<bool>::iterator stop);

    // Constants
    std::unique_ptr<const unsigned int> minimum_size;

    // Define inputs and parameters
    std::unique_ptr<unsigned int> t; // Counter for successful decompressions
    // Vectors

    public:
        PocketPlusDecompressor(std::unique_ptr<unsigned int>& vector_length){
            if ((*vector_length < 8) || (*vector_length > 65535)){ // ############ ToDo: check if this is still the case after the review
                throw std::out_of_range("8 <= input_vector_length <= 2^16-1 (65535");
            }
            minimum_size = std::make_unique<const unsigned int>(10); // ToDo: check the actual minimum size!
            
            t = std::make_unique<unsigned int>(0);
        }
        // Public functions
        std::deque<bool> decompress(std::deque<bool>& input);
};

};
};

#endif