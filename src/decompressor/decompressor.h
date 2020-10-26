#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <deque>
#include <memory>
#include <iterator>

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
    std::unique_ptr<unsigned int> input_vector_length; // F
    std::unique_ptr<unsigned int> robustness_level; // R_t
    std::unique_ptr<bool> d_t;

    // Vectors
    std::deque<std::deque<bool>> input_vector; // I
    std::deque<std::deque<bool>> mask_vector; // M
    std::deque<std::deque<bool>> change_vector; // D

    public:
        PocketPlusDecompressor(std::unique_ptr<unsigned int>& vector_length){
            if ((*vector_length < 8) || (*vector_length > 65535)){ // ############ ToDo: check if this is still the case after the review
                throw std::out_of_range("8 <= input_vector_length <= 2^16-1 (65535");
            }
            input_vector_length = std::make_unique<unsigned int>(*vector_length);
            minimum_size = std::make_unique<const unsigned int>(10); // ToDo: check the actual minimum size!
            
            t = std::make_unique<unsigned int>(0);
            std::deque<bool> initial_mask_vector;
            initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0 // ############ ToDo: Make initial mask user defined
            mask_vector.push_back(initial_mask_vector);
        }
        // Public functions
        std::deque<bool> decompress(std::deque<bool>& input);
};

};
};

#endif