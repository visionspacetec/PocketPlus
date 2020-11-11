#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <deque>
#include <memory>
#include <algorithm>
#include <cmath>

#include "pocketplusutils.h"

namespace pocketplus {
namespace compressor {

class PocketPlusCompressor{
    // Private functions
    std::deque<bool> count(const unsigned int&);
    std::deque<bool> run_length_encoding(const std::deque<bool>&);
    std::deque<bool> bit_extraction(const std::deque<bool>&, const std::deque<bool>&);
    unsigned int hamming_weight(const std::deque<bool>&);
    std::deque<bool> reverse(const std::deque<bool>&);
    std::deque<bool> inverse(const std::deque<bool>&);

    // Constants
    std::unique_ptr<const unsigned int> robustness_level_min;
    std::unique_ptr<const unsigned int> robustness_level_max;

    // Define inputs and parameters
    std::unique_ptr<unsigned int> t;
    std::unique_ptr<unsigned int> input_vector_length; // F // User defined value
    std::deque<bool> input_vector_length_count;
    std::deque<bool> initial_mask_vector;

    // Vectors
    std::deque<bool> mask_flag; // p_t
    std::deque<bool> input_old;
    std::deque<bool> mask_new; // M_t
    std::deque<bool> mask_old;
    std::deque<bool> mask_build_old;
    std::deque<bool> mask_build_new;
    
    std::deque<std::deque<bool>> mask_change_vector; // D
    std::deque<bool> mask_change_0; // D_0
    
    std::deque<bool> rle_mask_change;
    std::deque<bool> input_mask_bit_extraction;
    std::deque<bool> X_t;
    std::deque<bool> y_t;
    std::deque<bool> e_t;
    std::deque<bool> k_t;
    std::deque<bool> X_t_rle;
    std::deque<bool> robustness_level_bit_3;
    std::deque<bool> mask_shifted_rle;
    std::deque<bool> first_binary_vector; // h_t Mask change vector
    std::deque<bool> second_binary_vector; // q_t Information about entire mask vector
    std::deque<bool> third_binary_vector ; // u_t either unpredictable bits or the original input I_t
    std::deque<bool> output;
    public:
        PocketPlusCompressor(){
            robustness_level_min = std::make_unique<const unsigned int>(0);
            robustness_level_max = std::make_unique<const unsigned int>(7);
            t = std::make_unique<unsigned int>(0);
            /*
            input_vector_length = std::make_unique<unsigned int>(*vector_length); // F // User defined value
            input_vector_length_count = count(*input_vector_length);
            initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0 // ############ ToDo: Make initial mask user defined
            mask_new = initial_mask_vector;
            input_old.assign(*input_vector_length, 0);
            mask_old.assign(*input_vector_length, 0); // M_t
            mask_build_old.assign(*input_vector_length, 0); // B_t // B_0 = 0
            mask_build_new.assign(*input_vector_length, 0); // B_t // B_0 = 0
            mask_change_0.assign(*input_vector_length, 0); 
            */
        }
        // Public functions
        void set_input_vector_length(const unsigned int& vector_length);
        std::deque<bool> compress(
            const std::deque<bool>& input_new, 
            const unsigned int& robustness_level,
            const bool& new_mask_flag,
            const bool& send_mask_flag,
            const bool& uncompressed_flag
        );
};

};
};

#endif