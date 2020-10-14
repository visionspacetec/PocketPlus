#include <iostream>
#include <vector>
#include <cstddef>
#include <memory>
#include <algorithm>
#include <deque>
#include <cmath>

// 5.3.1.1 Counter encoding function
std::deque<bool> count(const unsigned int& a){
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
            output_vector.emplace_front((a >> i) & 1);
        }
        output_vector.insert(output_vector.begin(), {1, 1, 0});
    }
    else{
        auto length = std::make_unique<unsigned int>(2 * std::floor(std::log2(a - 2) + 1) - 6);
        for(unsigned int i = 0; i < *length; i++){
            output_vector.emplace_front((a >> i) & 1);
        }
        output_vector.insert(output_vector.begin(), {1, 1, 1});
    }
    return output_vector;
}

// 5.3.1.2 Run length encoding
// ToDo
std::deque<bool> run_length_encoding(const std::deque<bool>& a){
    std::deque<bool> output_vector;
    unsigned int zero_counter = 0;
    for(auto i = a.begin(); i != a.end(); i++){
        zero_counter++;
        if(!(*i == 0)){
            auto count_value = count(zero_counter);
            output_vector.insert(output_vector.end(), count_value.begin(), count_value.end());
            zero_counter = 0;
        }
    }
    return output_vector;
}

// 5.3.1.3 Bit extraction function
std::deque<bool> bit_extraction(const std::deque<bool>& a, const std::deque<bool>& b){
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
unsigned int hamming_weight(const std::deque<bool>& alpha){
    unsigned int output = 0;
    for(size_t i = 0; i < alpha.size(); i++){
        output += alpha.at(i);
    }
    return output;
}

// Reverse deque helper function, denoted < A > in the standard
std::deque<bool> reverse(const std::deque<bool>& a){
    std::deque<bool> output_vector;
    for(auto i = a.rbegin(); i != a.rend(); i++ ){
        output_vector.emplace_back(*i);
    }
    return output_vector;
}

// Inverse deque helper function, denoted ~A in the standard
std::deque<bool> inverse(const std::deque<bool>& a){
    std::deque<bool> output_vector;
    for(auto i = a.begin(); i != a.end(); i++ ){
        output_vector.emplace_back(!*i);
    }
    return output_vector;
}

int main(int argc, char* argv[]){
/*
    //#######################
    std::cout << "Inverse test: ";
    std::deque<bool> invtest = {1,0,0};
    for(auto i: invtest){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    for(auto i: inverse(invtest)){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    //#######################

    //#######################
    std::cout << "Reverse test: ";
    std::deque<bool> test = {1,0,0};
    for(auto i: test){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    for(auto i: reverse(test)){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    //#######################

    //#######################
    std::cout << "COUNT test: ";
    unsigned int testi = 3;
    for(auto i: count(testi)){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    //#######################

    //#######################
    std::cout << "Hamming weight test: ";
    std::deque<bool> testj = {0,1,1,0,1};
    for(auto j: testj){
        std::cout << j << " ";
    }
    std::cout << std::endl;
    std::cout << "Weight: " << hamming_weight(testj) << std::endl;
    //#######################

    //#######################
    std::cout << "Bit extraction function test: " << std::endl;
    std::deque<bool> testk = {1,1,0,0,1};
    std::cout << "a: ";
    for(auto j: testj){
        std::cout << j << " ";
    }
    std::cout << std::endl;
    std::cout << "b: ";
    for(auto k: testk){
        std::cout << k << " ";
    }
    std::cout << std::endl;
    auto extracted = bit_extraction(testj, testk);
    std::cout << "Result: " ;
    for(auto j: extracted){
        std::cout << j << " ";
    }
    std::cout << std::endl;
    //#######################

    //#######################
    std::cout << "Run length encoding test: ";
    std::deque<bool> test_rle = {1,1,0,0,1,0,0,0};
    for(auto j: test_rle){
        std::cout << j << " ";
    }
    std::cout << std::endl;
    auto rle = run_length_encoding(test_rle);
    for(auto j: rle){
        std::cout << j << " ";
    }
    std::cout << std::endl;
    //####################### */

    std::cout << "Welcome to PocketPlus in c++" << std::endl;

    // Define inputs and parameters
    auto t = std::make_unique<unsigned int>(0);
    auto input_vector_length = std::make_unique<unsigned int>(8); // F // User defined value
    std::deque<bool> initial_mask_vector;
    initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0
    auto robustness_level = std::make_unique<unsigned int>(1); // R_t // User defined value
    std::deque<bool> new_mask_flag; // p_t
    auto uncompressed_flag = std::make_unique<bool>(); // r_t
    auto send_mask_flag = std::make_unique<bool>(); // n_t
    auto send_input_length_flag = std::make_unique<bool>(); // v_t

    auto d_t = std::make_unique<bool>();

    // Vectors
    std::deque<bool> input_new;
    std::deque<bool> input_old;
    input_old.assign(*input_vector_length, 0);
    std::deque<bool> mask_new = initial_mask_vector; // M_t
    std::deque<bool> mask_old;
    mask_old.assign(*input_vector_length, 0); // M_t
    std::deque<bool> mask_build_old;
    mask_build_old.assign(*input_vector_length, 0); // B_t // B_0 = 0
    std::deque<bool> mask_build_new;
    mask_build_new.assign(*input_vector_length, 0); // B_t // B_0 = 0
    std::deque<bool> mask_change;
    mask_change.assign(*input_vector_length, 0); // D_t
    std::deque<bool> rle_mask_change;
    std::deque<bool> first_binary_vector; // h_t Mask change vector
    std::deque<bool> second_binary_vector; // q_t Information about entire mask vector
    std::deque<bool> third_binary_vector ; // u_t either unpredictable bits or the original input I_t
    std::deque<bool> output;

    // Set inputs and parameters
    if ((*input_vector_length < 8) || (*input_vector_length > 65535)){
        throw std::out_of_range("8 <= input_vector_length <= 2^16-1 (65535");
    }

    if ((*robustness_level < 0) || (*robustness_level > 7)){
        throw std::out_of_range("0 <= robustness_level <= 7");
    }

    new_mask_flag.resize(*robustness_level + 1);
    new_mask_flag.at(0) = false; // User defined value
    if((*robustness_level > 0) && new_mask_flag.at(0) == true){
        std::fill(new_mask_flag.begin()+1, new_mask_flag.end(), 0);
    }
    std::cout << "new_mask_flag: ";
    for(auto i: new_mask_flag){
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "initial_mask_vector: ";
    for(auto i: initial_mask_vector){
        std::cout << (int) i << " ";
    }
    std::cout << std::endl;

    send_mask_flag = std::make_unique<bool>(0); // n_t // n_0 = 0
    uncompressed_flag = std::make_unique<bool>(1); // r_t

    if(!*send_mask_flag && !*uncompressed_flag){
        throw std::invalid_argument("if send_mask_flag == false -> uncompressed_flag != true");
    }

    send_input_length_flag = std::make_unique<bool>(0); // v_t // v_0 = 0

    // Section 4.2 Mask update
    auto first_packet = std::make_unique<bool>(1);
    
    // Section 4.2.1
    if(!*first_packet && !new_mask_flag.at(0)){
        std::generate(
            mask_build_new.begin(), 
            mask_build_new.end(), 
            [mask_build_old, input_new, input_old, n = 0]() mutable {
                auto out = (input_new.at(n) ^ input_old.at(n)) | mask_build_old.at(n);
                n++;
                return out;
            }
        );
    }
    else{
        std::fill(mask_build_new.begin(), mask_build_new.end(), 0);
        if(*first_packet){
            first_packet = std::make_unique<bool>(false);
        }
    }
    // Section 4.2.2
    if(!first_packet){
        if(!new_mask_flag.at(0)){
            std::generate(
                mask_new.begin(), 
                mask_new.end(), 
                [mask_old, input_new, input_old, n = 0]() mutable {
                    auto out = (input_new.at(n) ^ input_old.at(n)) | mask_old.at(n);
                    n++;
                    return out;
                }
            );
        }
        else{
            std::generate(
                mask_new.begin(), 
                mask_new.end(), 
                [mask_build_old, input_new, input_old, n = 0]() mutable {
                    auto out = (input_new.at(n) ^ input_old.at(n)) | mask_build_old.at(n);
                    n++;
                    return out;
                }
            );
        }
        // Section 4.2.3
        std::generate(
            mask_change.begin(), 
            mask_change.end(), 
            [mask_new, mask_old, n = 0]() mutable {
                auto out = mask_new.at(n) ^ mask_old.at(n);
                n++;
                return out;
            }
        );
    }

    // 5.3.2 Encoding step
    // 5.3.2.1
    if(!*send_mask_flag && !*uncompressed_flag){
        d_t = std::make_unique<bool>(1);
    }
    else{
        d_t = std::make_unique<bool>(0);
    }

    // 5.3.2.2 first binary vector
    if(!send_mask_flag){
        first_binary_vector = {1, 0, 0, 0, 0, 0};
    }
    else{
        std::deque<bool> X_t;
        if(*robustness_level == 0){
            X_t = reverse(mask_change);
        }
        else if((int)(*t - *robustness_level) <= 0){
            // D is mask_change
            // X_t = [<D_1 OR D_2 OR ... OR D_t>]
        }
        else{
            // X_t = [<D_(t-robustness_level) OR D_(t-robustness_level)+1 OR ... OR D_t>]
        }

        std::deque<bool> y_t;
        y_t = bit_extraction(inverse(mask_new), reverse(X_t));

        std::deque<bool> e_t;
        if((*robustness_level == 0) || (hamming_weight(X_t) == 0)){}
        else if ((hamming_weight(y_t) == 0) && (*robustness_level > 0) && (hamming_weight(X_t) != 0)){
            e_t = {0};
        }
        else{
            e_t = {1};
        }

        std::deque<bool> k_t;
        if(!((*robustness_level == 0) || (hamming_weight(X_t) == 0) || (hamming_weight(y_t) == 0))){
            k_t = bit_extraction(inverse(mask_new), reverse(X_t));
        }

        //first_binary_vector = [RLE(X_t), '10', BIT_3(robustness_level), e_t, k_t, d_t]
        auto X_t_rle = run_length_encoding(X_t);
        first_binary_vector.insert(first_binary_vector.end(), X_t_rle.begin(), X_t_rle.end());
        first_binary_vector.insert(first_binary_vector.end(), {1, 0});
        std::deque<bool> robustness_level_bit_3;
        for(unsigned int i = 0; i < 3; i++){
            robustness_level_bit_3.emplace_front((*robustness_level >> i) & 1);
        }
        first_binary_vector.insert(first_binary_vector.end(), robustness_level_bit_3.begin(), robustness_level_bit_3.end());
        first_binary_vector.insert(first_binary_vector.end(), e_t.begin(), e_t.end());
        first_binary_vector.insert(first_binary_vector.end(), k_t.begin(), k_t.end());
        first_binary_vector.emplace_back(*d_t);
    }

    // 5.3.2.3 second binary vector
    // ToDo
    // Second vector: ['1', RLE(mask), '10']

    // 5.3.2.4 third binary vector
    // ToDo
    //if uncompressed_flag==1 AND send_input_length_flag==0
    //then ['10' input_new, send_changes_flag]
    //if uncompressed_flag==1 AND send_input_length_flag==1
    //then ['11', COUNT(F), input_new, send_changes_flag]
    //if uncompressed_flag==0
    //then ['unpredictable bit values defined by the mask']
    //or, if uncompressed_flag not already indicated by common_settings_flag in h_t (first vector): 
    //[uncompressed_flag, 'unpredictable bit values defined by the mask']

    // Output vector: [first, second, third]
    output.insert(output.end(), first_binary_vector.begin(), first_binary_vector.end());
    output.insert(output.end(), second_binary_vector.begin(), second_binary_vector.end());
    output.insert(output.end(), third_binary_vector.begin(), third_binary_vector.end());

    // Compress more ...

    // 5.4 Termination
    // 5.4.1.1
    auto w_t = count(*input_vector_length + 1);
    output.insert(output.end(), w_t.begin(), w_t.end());
}
