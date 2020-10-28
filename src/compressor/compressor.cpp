#include "compressor.h"

using namespace pocketplus::compressor;

// 5.3.1.1 Counter encoding function
std::deque<bool> PocketPlusCompressor::count(const unsigned int& a){
    std::deque<bool> output_vector;
    if((a == 0) || (a >= 65536)){
        throw std::out_of_range("1 <= a <= 2^16 = 65536");
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
        auto length = std::make_unique<unsigned int>(2 * std::floor(std::log2(a - 2) + 1) - 6);
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
    std::unique_ptr<unsigned int>& robustness_level, // R_t // User defined value
    std::unique_ptr<bool>& new_mask_flag,
    std::unique_ptr<bool>& send_mask_flag,
    std::unique_ptr<bool>& uncompressed_flag,
    std::unique_ptr<bool>& send_changes_flag,
    std::unique_ptr<bool>& send_input_length_flag
    ){
    if ((*robustness_level < *robustness_level_min) || (*robustness_level > *robustness_level_max)){
        throw std::out_of_range("0 <= robustness_level <= 7");
    }
    if(!*send_mask_flag && !*uncompressed_flag){
        throw std::invalid_argument("if send_mask_flag == false -> uncompressed_flag != true");
    }
    if(*send_changes_flag && (*t == 0)){
        throw std::invalid_argument("if send_changes_flag == true -> t > 0");
    }

    first_binary_vector.clear();
    second_binary_vector.clear();
    third_binary_vector.clear();
    output.clear();

    // ToDo handle the new mask flag correctly ##############################
    mask_flag.resize(*robustness_level + 1);
    mask_flag.at(0) = *new_mask_flag; // User defined value
    if((*robustness_level > 0) && mask_flag.at(0) == true){
        std::fill(mask_flag.begin()+1, mask_flag.end(), 0);
    }

    // Section 4.2 Mask update    
    // Section 4.2.1
    if(!(*t == 0) && !mask_flag.at(0)){
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
        if(!mask_flag.at(0)){
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
    std::cout << "All mask change vectors:" << std::endl;
    for(auto vec: mask_change_vector){
        pocketplus::utils::print_vector(vec);
    }
    // 5.3.2 Encoding step
    // 5.3.2.1
    std::unique_ptr<bool> d_t;
    if(!*send_mask_flag && !*uncompressed_flag){
        d_t = std::make_unique<bool>(1);
    }
    else{
        d_t = std::make_unique<bool>(0);
    }
    // 5.3.2.2 first binary vector
    if(!*send_changes_flag){
        first_binary_vector = {1, 0, 0, 0, 0, 0};
    }
    else{
        X_t.resize(*input_vector_length);
        if(*robustness_level == 0){
            X_t = reverse(mask_change_vector.at(0));
        }
        else if((int)(*t - *robustness_level) <= 0){
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
            for(unsigned int i = 0; i < *robustness_level + 1; i++){
                for(unsigned int index = 0; index < mask_change_vector.at(i).size(); index++){
                    X_t.at(index) = mask_change_vector.at(i).at(index) || X_t.at(index);
                }
                std::cout << "mask_change_vector.at(i)" << std::endl;
                pocketplus::utils::print_vector(mask_change_vector.at(i));
            }
            X_t = reverse(X_t); // ToDo: Just go the other way thru the for loop to avoid this step
            std::cout << "X_t case 3" << std::endl;
        }

        y_t = bit_extraction(inverse(mask_new), reverse(X_t));

        std::cout << "mask_new:" << std::endl;
        pocketplus::utils::print_vector(mask_new);

        if((*robustness_level == 0) || (hamming_weight(X_t) == 0)){}
        else if ((hamming_weight(y_t) == 0) && (*robustness_level > 0) && (hamming_weight(X_t) != 0)){
            e_t = {0};
        }
        else{
            e_t = {1};
        }

        if(!((*robustness_level == 0) || (hamming_weight(X_t) == 0) || (hamming_weight(y_t) == 0))){
            k_t = bit_extraction(inverse(mask_new), reverse(X_t));
        }

        //first_binary_vector = [RLE(X_t), '10', BIT_3(robustness_level), e_t, k_t, d_t]
        std::cout << "X_t: " << std::endl;
        pocketplus::utils::print_vector(X_t);
        X_t_rle = run_length_encoding(X_t);
        std::cout << "X_t_rle: " << std::endl;
        pocketplus::utils::print_vector(X_t_rle);
        first_binary_vector.insert(first_binary_vector.end(), X_t_rle.begin(), X_t_rle.end());
        first_binary_vector.insert(first_binary_vector.end(), {1, 0});
        robustness_level_bit_3 = std::deque<bool>();
        for(unsigned int i = 0; i < 3; i++){
            robustness_level_bit_3.emplace_front((*robustness_level >> i) & 1);
        }
        
        first_binary_vector.insert(first_binary_vector.end(), robustness_level_bit_3.begin(), robustness_level_bit_3.end());
        first_binary_vector.insert(first_binary_vector.end(), e_t.begin(), e_t.end());
        first_binary_vector.insert(first_binary_vector.end(), k_t.begin(), k_t.end());
        pocketplus::utils::print_vector(first_binary_vector);
        first_binary_vector.emplace_back(*d_t);
        pocketplus::utils::print_vector(first_binary_vector);
    }

    // 5.3.2.3 Second binary vector
    // Second vector: ['1', RLE(< mask ^ mask_<< >), '10']
    if(*d_t == 1){
        second_binary_vector = std::deque<bool>();
    }
    else if(*send_mask_flag == 1){
        second_binary_vector = std::deque<bool>();
        second_binary_vector.insert(second_binary_vector.end(), {1});
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
        second_binary_vector.insert(second_binary_vector.end(), mask_shifted_rle.begin(), mask_shifted_rle.end());
        second_binary_vector.insert(second_binary_vector.end(), {1, 0});
    }
    else{
        second_binary_vector = std::deque<bool>({0});
    }

    // 5.3.2.4 Third binary vector
    third_binary_vector = std::deque<bool>();
    if(*d_t == 1){
        input_mask_bit_extraction = bit_extraction(input_new , mask_new);
        third_binary_vector.insert(third_binary_vector.end(), input_mask_bit_extraction.begin(), input_mask_bit_extraction.end());
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 1)){
        third_binary_vector.insert(third_binary_vector.end(), {1, 1});
        third_binary_vector.insert(third_binary_vector.end(), input_vector_length_count.begin(), input_vector_length_count.end());
        third_binary_vector.insert(third_binary_vector.end(), input_new.begin(), input_new.end());
        third_binary_vector.insert(third_binary_vector.end(), {*send_mask_flag});
    }
    else if((*uncompressed_flag == 1) && (*send_input_length_flag == 0)){
        third_binary_vector.insert(third_binary_vector.end(), {1, 0});
        third_binary_vector.insert(third_binary_vector.end(), input_new.begin(), input_new.end());
        third_binary_vector.insert(third_binary_vector.end(), {*send_mask_flag});
    }
    else{
        third_binary_vector.insert(third_binary_vector.end(), {0});
        input_mask_bit_extraction = bit_extraction(input_new , mask_new);
        third_binary_vector.insert(third_binary_vector.end(), input_mask_bit_extraction.begin(), input_mask_bit_extraction.end());
    }

    // Output vector: [first, second, third]
    output.insert(output.end(), first_binary_vector.begin(), first_binary_vector.end());
    output.insert(output.end(), second_binary_vector.begin(), second_binary_vector.end());
    output.insert(output.end(), third_binary_vector.begin(), third_binary_vector.end());

    t = std::make_unique<unsigned int>(*t + 1); 
    input_old = input_new;
    mask_old = mask_new;
    mask_build_old = mask_build_new;

    std::cout << "First:" << std::endl;
    pocketplus::utils::print_vector(first_binary_vector);
    std::cout << "Second:" << std::endl;
    pocketplus::utils::print_vector(second_binary_vector);
    std::cout << "Third:" << std::endl;
    pocketplus::utils::print_vector(third_binary_vector);

    return output;
}

std::deque<bool> PocketPlusCompressor::add_termination_sequence(const std::deque<bool>& in){
    // 5.4 Termination
    // 5.4.1.1
    auto w_t = count(*input_vector_length + 1);
    output.insert(output.end(), w_t.begin(), w_t.end());
    return output;
}