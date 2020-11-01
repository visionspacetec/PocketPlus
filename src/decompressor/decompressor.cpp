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
    while(!((*it == 1) && (*(it + 1) == 0))){ // '1' '0' indicates the end of the RLE
        // Revert COUNT operation
        if(*it == 0){
            out.emplace_back(1);
            it += 1;
            in.pop_front();
        }
        else if(hamming_weight_in_range(it, it + 2) == 2){
            it += 3;
            pocketplus::utils::pop_n_from_front(in, 3);
            // Undo BIT_5(A - 2)
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
        else if(hamming_weight_in_range(it, it + 2) == 3){ // ToDo ### Check if working!
            std::cout << "count_tmp>=34" << std::endl;
            it += 3;
            pocketplus::utils::pop_n_from_front(in, 3);
            // Undo BIT_E(A - 2)
            auto count_size = std::make_unique<unsigned int>(5);
            while(!*it){
                *count_size += 1;
                it++;
                in.pop_front();
            }
            std::cout << "COUNT size " << *count_size << std::endl;
            auto count_tmp = std::make_unique<unsigned int>(0);
            auto bit_shift = std::make_unique<unsigned int>(0);
            for(auto ite = it + *count_size; ite >= it; ite--, *bit_shift += 1){
                if(*ite){
                    *count_tmp |= 1 << *bit_shift;
                }
            }
            *count_tmp += 2;
            it += *count_size + 1;
            pocketplus::utils::pop_n_from_front(in, *count_size + 1);
            std::cout << "count_tmp=" << *count_tmp << std::endl;
            for(unsigned int i = 0; i < *count_size + 1; i++){
                out.emplace_front(((*count_tmp) >> i) & 1);
            }
        }
        else{
            throw std::invalid_argument("Revert of COUNT(X) failed");
        }
    }
}

// Tries to decompress a packet from a boolean deque and removes it from the input
std::deque<bool> PocketPlusDecompressor::decompress(std::deque<bool>& input){
    std::deque<bool> output;

    // Perform basic plausability check
    if(input.empty()){
        return output;
    }

    // To check for stuffed zeros the initial and processed length are compared
    *input_vector_size_before_processing = input.size();

    // Initialize bit iterator
    auto bit_position = input.begin();

    // Check if there are enough bits left for a successful decode
    if(input.size() < *minimum_size){
        input.clear();
        return output;
    }

    // Process first sub vector
    // 5.3.2.2
    std::unique_ptr<bool> send_changes_flag;
    std::unique_ptr<bool> uncompressed_flag;
    std::unique_ptr<bool> send_mask_flag;
    std::unique_ptr<bool> d_t;
    std::deque<bool> D_t;
    std::deque<bool> k_t;
    std::deque<bool> y_t;

    if(std::equal(bit_position, bit_position + 5, check_send_changes_flag.begin())){
        std::cout << "send_changes_flag (n_t) = 0" << std::endl;
        send_changes_flag = std::make_unique<bool>(0);
        std::cout << "uncompressed_flag (r_t) = 1" << std::endl;
        uncompressed_flag = std::make_unique<bool>(1); // From the standard: if nt = 0 -> rt = 1
        bit_position += 6;
        pocketplus::utils::pop_n_from_front(input, 6);
        std::cout << "d_t = 0" << std::endl;
        d_t = std::make_unique<bool>(0);
    }
    else{
        send_changes_flag = std::make_unique<bool>(1);
        std::cout << "send_changes_flag (n_t) = 1" << std::endl;
        std::deque<bool> X_t;
        if(!((*bit_position == 1) && (*(bit_position + 1) == 0))){
            undo_rle(input, X_t, bit_position);
        }

        while(X_t.size() < *input_vector_length){
            X_t.emplace_back(0);
        }
        std::cout << "X_t: ";
        pocketplus::utils::print_vector(X_t);

        if(X_t.size() == 0){
            D_t.assign(*input_vector_length, 0);
            change_vector.push_back(D_t);
        }
        else{
            D_t = reverse(X_t);
            std::cout << "D_t: ";
            pocketplus::utils::print_vector(D_t);
        }
        auto X_t_weight = std::make_unique<unsigned int>(hamming_weight_in_range(X_t.begin(), X_t.end()));
        std::cout << "X_t_weight: " << *X_t_weight << std::endl;
        std::cout << "End of RLE!" << std::endl;
        bit_position += 2;
        pocketplus::utils::pop_n_from_front(input, 2);
        // Undo BIT_3(robustness_level)
        robustness_level = std::make_unique<unsigned int>(0);
        auto bit_shift = std::make_unique<unsigned int>(0);
        for(auto it = bit_position + 2; it >= bit_position; it--, *bit_shift += 1){
            if(*it){
                *robustness_level |= 1 << *bit_shift;
            }
        }
        std::cout << "Robustness level: " << *robustness_level << std::endl;
        bit_position += 3;
        pocketplus::utils::pop_n_from_front(input, 3);
        std::deque<bool> e_t;
        
        if((*robustness_level == 0) || (*X_t_weight == 0)){
            // e_t is empty
        }
        else{
            if(input.front() == 0){
                e_t.emplace_back(0);
                bit_position += 1;
                input.pop_front();
                for(auto i = 0; i < *X_t_weight; i++){
                    y_t.emplace_back(0);
                }
            }
            else{
                e_t.emplace_back(1);
                bit_position += 1;
                input.pop_front();
            }
        }
        std::cout << "y_t.size(): " << y_t.size() << std::endl;
        if(e_t.size() == 0){
            std::cout << "e_t: EMPTY" << std::endl;
        }
        else{
            std::cout << "e_t: " << e_t.at(0) << std::endl;
        }
        if((*robustness_level == 0) || (*X_t_weight == 0) || (y_t.size() > 0)){
            // k_t is empty
        }
        else{
            // k_t = BE( ~M_t, <X_t> )
            std::cout << "X_t_weight: " << *X_t_weight << std::endl;
            for(auto i = 0; i < *X_t_weight; i++){
                *bit_position += 1;
                k_t.emplace_back(input.front());
                input.pop_front();
            }
            std::cout << "k_t: " << std::endl;
            pocketplus::utils::print_vector(k_t);
        }
        std::cout << "k_t.size(): " << k_t.size() << std::endl;
        
        d_t = std::make_unique<bool>(input.front());
        bit_position += 1;
        input.pop_front();
        std::cout << "d_t: " << *d_t << std::endl;
        if(*d_t == 1){
            send_mask_flag = std::make_unique<bool>(0);
            uncompressed_flag = std::make_unique<bool>(0);
            std::cout << "send_mask_flag (f_t) = 0" << std::endl;
            std::cout << "uncompressed_flag (r_t) = 0" << std::endl;
        }
    }

    // Process second sub vector
    std::cout << "Second vector" << std::endl;
    std::unique_ptr<bool> send_input_length_flag;
    if(*t == 0){
        //std::cout << "d_t = 0" << std::endl;
        //d_t = std::make_unique<bool>(0);
        std::cout << "send_input_length_flag (v_t) = 1" << std::endl;
        send_input_length_flag = std::make_unique<bool>(1);
        std::cout << "send_mask_flag (f_t) = 0" << std::endl;
        send_mask_flag = std::make_unique<bool>(0);
    }

    // 5.3.2.3
    std::deque<bool> q_t;
    if(*d_t == 1){
        // q_t is empty
        std::deque<bool> M_t = mask_vector.back();
        //build mask from old mask and mask changes D_t with values k_t inserted
        auto it_D_t = D_t.begin();
        auto it_M_t = M_t.begin();
        auto it_old_mask = mask_vector.back().begin();
        std::cout << "k_t: " << std::endl;
        pocketplus::utils::print_vector(k_t);
        std::cout << "y_t: " << std::endl;
        pocketplus::utils::print_vector(y_t);
        if(y_t.size() > 0){
            auto it_y_t = y_t.begin();
            while(it_M_t != M_t.end()){
                if(*it_D_t){
                    *it_M_t = !*it_y_t; // Undo ~M_t
                    it_y_t++;
                }
                it_D_t++;
                it_M_t++;
                it_old_mask++;
            }
        }
        else if(k_t.size() > 0){
            auto it_k_t = k_t.begin();
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
        mask_vector.push_back(M_t);
        std::cout << "M_t: " << std::endl;
        pocketplus::utils::print_vector(M_t);
    }
    else{
        if(input.front() == 1){ // send_mask_flag (f_t) = 1
            send_mask_flag = std::make_unique<bool>(1);
            std::cout << "send_mask_flag (f_t) = 1" << std::endl;
            // '1' | RLE(<(M_t XOR M_t<<))>) | '10'
            bit_position++;
            input.pop_front();
            std::deque<bool> mask_mask_shifted;
            if(!((*bit_position == 1) && (*(bit_position + 1) == 0))){
                undo_rle(input, mask_mask_shifted, bit_position);
                std::deque<bool> M_t;
                auto it = mask_mask_shifted.rbegin();
                auto it_M_t = M_t.rbegin();
                if(*it == 1){
                    M_t.emplace_front(1);
                }
                else{
                    M_t.emplace_front(0);
                }
                it++;
                for(; it != mask_mask_shifted.rend() - 1; it++){
                    if((*it == 1) && (*it_M_t == 0)){
                        M_t.emplace_front(1);
                    }
                    else if((*it == 0) && (*it_M_t == 1)){
                        M_t.emplace_front(1);
                    }
                    else{
                        M_t.emplace_front(0);
                    }
                    it_M_t++;
                }
                M_t = reverse(M_t);
                while(M_t.size() < *input_vector_length){
                    M_t.emplace_front(0);
                }
                mask_vector.push_back(M_t);
                std::cout << "M_t:" << std::endl;
                pocketplus::utils::print_vector(M_t);
            }
            else{ // RLE(<(M_t XOR M_t<<))>) == NULL
                std::deque<bool> M_t;
                M_t.assign(*input_vector_length, 0);
                mask_vector.push_back(M_t);
            }
            std::cout << "End of RLE!" << std::endl;
            bit_position += 2;
            pocketplus::utils::pop_n_from_front(input, 2);
        }
        else{ // otherwise
            send_mask_flag = std::make_unique<bool>(0);
            std::cout << "send_mask_flag (f_t) = 0" << std::endl;
            bit_position++;
            input.pop_front();
        }
    }

    // Process third sub vector
    // 5.3.2.4
    if(*t > 0){
        if(*d_t == 1){
            // Do nothing
        }
        else if(input.front() == 1){ // r_t = 1
            std::cout << "uncompressed_flag (r_t) = 1" << std::endl;
            uncompressed_flag = std::make_unique<bool>(1);
            if(*(input.begin() + 1) == 1){ // v_t = 1
                std::cout << "send_input_length_flag (v_t) = 1" << std::endl;
                send_input_length_flag = std::make_unique<bool>(1);
            }
            else{ // v_t = 0
                std::cout << "send_input_length_flag (v_t) = 0" << std::endl;
                send_input_length_flag = std::make_unique<bool>(0);
            }
        }
        else{
            std::cout << "uncompressed_flag (r_t) = 0" << std::endl;
            uncompressed_flag = std::make_unique<bool>(0);
        }
    }
    if(*d_t == 1){ // d_t = 1
        output.assign(*input_vector_length, 0); // Fill the output vector with zeros
        auto it_mask = mask_vector.back().begin();
        auto it_output = output.begin();
        auto it_last_input = input_vector.back().begin();
        for(; it_mask != mask_vector.back().end();){
            if(*it_mask == 1){
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
        std::cout << "Extracted:" << std::endl;
        pocketplus::utils::print_vector(output);
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 1)){ // rt = 1 and vt = 1
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
            bit_position += 3;
            pocketplus::utils::pop_n_from_front(input, 3);
            // Undo BIT_E(A - 2)
            auto count_size = std::make_unique<unsigned int>(5);
            while(!*bit_position){
                *count_size += 1;
                bit_position++;
                input.pop_front();
            }
            std::cout << "COUNT size " << *count_size << std::endl;
            input_vector_length = std::make_unique<unsigned int>(0);
            auto bit_shift = std::make_unique<unsigned int>(0);
            for(auto ite = bit_position + *count_size; ite >= bit_position; ite--, *bit_shift += 1){
                if(*ite){
                    *input_vector_length |= 1 << *bit_shift;
                }
            }
            *input_vector_length += 2;
            bit_position += *count_size + 1;
            pocketplus::utils::pop_n_from_front(input, *count_size + 1);
            std::cout << "input_vector_length=" << *input_vector_length << std::endl;
        }
        else{
            throw std::invalid_argument("Revert of COUNT(input_vector_length) failed");
        }
        if(input.size() >= *input_vector_length){
            output.insert(output.end(), std::make_move_iterator(input.begin()), std::make_move_iterator(input.begin() + *input_vector_length));
            std::cout << "Extracted:" << std::endl;
            pocketplus::utils::print_vector(output);
            input.erase(input.begin(), input.begin() + *input_vector_length );
            input_vector.push_back(output);
        }
        else{
            throw std::invalid_argument("Not enough bits left to extract data frame of length " + std::to_string(*input_vector_length) + " and n_t");
        }
        *send_changes_flag = input.front();
        input.pop_front();
        std::cout << "send_changes_flag (n_t) = " << *send_changes_flag << std::endl;
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 0)){ // rt = 1 and vt = 0
        if(hamming_weight_in_range(bit_position, bit_position + 1) == 1){
            bit_position += 2;
            pocketplus::utils::pop_n_from_front(input, 2);
        }
        else{
            throw std::invalid_argument("Something went wrong");
        }
        if(input.size() >= *input_vector_length){
            output.insert(output.end(), std::make_move_iterator(input.begin()), std::make_move_iterator(input.begin() + *input_vector_length));
            std::cout << "Extracted:" << std::endl;
            pocketplus::utils::print_vector(output);
            input.erase(input.begin(), input.begin() + *input_vector_length );
            input_vector.push_back(output);
        }
        else{
            throw std::invalid_argument("Not enough bits left to extract data frame of length " + std::to_string(*input_vector_length) + " and n_t");
        }
        *send_changes_flag = input.front();
        input.pop_front();
        std::cout << "send_changes_flag (n_t) = " << *send_changes_flag << std::endl;
    }
    else{
        // BE(I_t, M_t) values of I_t at the positions where M_t is one
        // Evaluate mask and add bits in predictable positions from previous input_vector
        bit_position += 1;
        input.pop_front();
        output.assign(*input_vector_length, 0); // Fill the output vector with zeros
        auto it_mask = mask_vector.back().begin();
        auto it_output = output.begin();
        auto it_last_input = input_vector.back().begin();
        for(; it_mask != mask_vector.back().end();){
            if(*it_mask == 1){
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
        std::cout << "Extracted:" << std::endl;
        pocketplus::utils::print_vector(output);
    }
    *t += 1;
    pocketplus::utils::pop_n_from_front(input, (8 - ((*input_vector_size_before_processing - input.size()) % 8)) % 8);
    std::cout << "INPUT remaining:" << std::endl;
    pocketplus::utils::print_vector(input);
    return output;
}