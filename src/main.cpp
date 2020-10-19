#include <iostream>
#include <vector>
#include <cstddef>
#include <memory>
#include <algorithm>
#include <deque>
#include <cmath>
#include <fstream>
#include <iterator>

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

    std::deque<bool> mask_flag; // p_t

    // Vectors
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
        PocketPlusCompressor(std::unique_ptr<unsigned int>& vector_length){
            if ((*vector_length < 8) || (*vector_length > 65535)){
                throw std::out_of_range("8 <= input_vector_length <= 2^16-1 (65535");
            }
            robustness_level_min = std::make_unique<const unsigned int>(0);
            robustness_level_max = std::make_unique<const unsigned int>(7);

            t = std::make_unique<unsigned int>(0);
            input_vector_length = std::make_unique<unsigned int>(*vector_length); // F // User defined value
            input_vector_length_count = count(*input_vector_length);
            initial_mask_vector.assign(*input_vector_length, 0); // M_0 = 0 // ############ ToDo: Make initial mask user defined
            mask_new = initial_mask_vector;
            input_old.assign(*input_vector_length, 0);

            input_old.assign(*input_vector_length, 0);
            mask_old.assign(*input_vector_length, 0); // M_t
            mask_build_old.assign(*input_vector_length, 0); // B_t // B_0 = 0
            mask_build_new.assign(*input_vector_length, 0); // B_t // B_0 = 0
            mask_change_0.assign(*input_vector_length, 0); 
        }
        // Public functions
        std::deque<bool> compress(
            const std::deque<bool>& input_new, 
            std::unique_ptr<unsigned int>& robustness_level,
            std::unique_ptr<bool>& new_mask_flag,
            std::unique_ptr<bool>& send_mask_flag,
            std::unique_ptr<bool>& uncompressed_flag,
            std::unique_ptr<bool>& send_changes_flag,
            std::unique_ptr<bool>& send_input_length_flag
        );
        std::deque<bool> add_termination_sequence(const std::deque<bool>&);
};

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
std::deque<bool> PocketPlusCompressor::run_length_encoding(const std::deque<bool>& a){
    std::deque<bool> output_vector;
    std::deque<bool> count_value;
    auto zero_counter = std::make_unique<unsigned int>(0);
    for(auto i = a.begin(); i != a.end(); i++){
        zero_counter = std::make_unique<unsigned int>(*zero_counter + 1);
        if(!(*i == 0)){
            count_value = count(*zero_counter);
            output_vector.insert(output_vector.end(), count_value.begin(), count_value.end());
            zero_counter = std::make_unique<unsigned int>(0);
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
unsigned int PocketPlusCompressor::hamming_weight(const std::deque<bool>& alpha){
    unsigned int output = 0;
    for(size_t i = 0; i < alpha.size(); i++){
        output += alpha.at(i);
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
    mask_change_vector.push_back(mask_change_0);

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
        }
        else{
            // X_t = [<D_(t-robustness_level) OR D_(t-robustness_level)+1 OR ... OR D_t>]
            X_t.assign(*input_vector_length, 0); 
            for(unsigned int i = 0; i < *robustness_level + 1; i++){
                for(unsigned int index = 0; index < mask_change_vector.at(i).size(); index++){
                    X_t.at(index) = mask_change_vector.at(i).at(index) || X_t.at(index);
                }
            }
        }

        y_t = bit_extraction(inverse(mask_new), reverse(X_t));

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
        X_t_rle = run_length_encoding(X_t);
        first_binary_vector.insert(first_binary_vector.end(), X_t_rle.begin(), X_t_rle.end());
        first_binary_vector.insert(first_binary_vector.end(), {1, 0});
        robustness_level_bit_3 = std::deque<bool>();
        for(unsigned int i = 0; i < 3; i++){
            robustness_level_bit_3.emplace_front((*robustness_level >> i) & 1);
        }
        
        first_binary_vector.insert(first_binary_vector.end(), robustness_level_bit_3.begin(), robustness_level_bit_3.end());
        first_binary_vector.insert(first_binary_vector.end(), e_t.begin(), e_t.end());
        first_binary_vector.insert(first_binary_vector.end(), k_t.begin(), k_t.end());
        first_binary_vector.emplace_back(*d_t);
    }

    // 5.3.2.3 Second binary vector
    // Second vector: ['1', RLE(< mask ^ mask_<< >), '10']
    if(*d_t == 1){
        std::cout << "d_t: " << *d_t << std::endl;
        second_binary_vector = std::deque<bool>();
    }
    else if(*send_mask_flag == 1){
        std::cout << "send_mask_flag: " << *send_mask_flag << std::endl;
        second_binary_vector = std::deque<bool>();
        second_binary_vector.insert(second_binary_vector.end(), {1});
        mask_shifted_rle = mask_new;
        mask_shifted_rle.pop_front();
        mask_shifted_rle.push_back({0});
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

    return output;
}

std::deque<bool> PocketPlusCompressor::add_termination_sequence(const std::deque<bool>& in){
    // 5.4 Termination
    // 5.4.1.1
    auto w_t = count(*input_vector_length + 1);
    output.insert(output.end(), w_t.begin(), w_t.end());
    return output;
}

void print_vector(const std::deque<bool>& in){
    for(auto i: in){
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

std::deque<bool> number_to_deque_bool(std::unique_ptr<long int>& input, std::unique_ptr<unsigned int>& length){
    std::deque<bool> out;
    for(unsigned int i = 0; i < *length; i++){
        out.emplace_front((*input >> i) & 1);
    }
    return out;
}

std::size_t divide_up(std::size_t dividend, std::size_t divisor){
    return (dividend + divisor - 1) / divisor;
}

std::string bool_to_string(std::deque<bool> const& boolvector){
    std::string ret(divide_up(boolvector.size(), 8), 0);
    auto counter = 0;
    auto out = ret.rbegin(); //big endian
    //auto out = ret.begin();     // little endian
    int shift = 0;
    for(auto it = boolvector.rbegin(); it != boolvector.rend(); it++){ //big
    //for(auto it = boolvector.begin(); it != boolvector.end(); it++){ // little
        *out |= *it << shift;
        if(++shift == 8){
            ++out;
            shift = 0;
        }
        counter++;
    }
    return ret;
}

int main(int argc, char* argv[]){

    std::cout << "Welcome to PocketPlus in c++" << std::endl;

    auto input_vector_length = std::make_unique<unsigned int>(32); // In bits

    PocketPlusCompressor compressor(input_vector_length);

    auto robustness_level = std::make_unique<unsigned int>(1); // R_t
    auto new_mask_flag = std::make_unique<bool>(1);            // p_t
    auto send_mask_flag = std::make_unique<bool>(0);           // f_t // f_0 = 0
    auto uncompressed_flag = std::make_unique<bool>(1);        // r_t // if n_t == 0 -> r_t = 1 --> r_0 = 1
    auto send_changes_flag = std::make_unique<bool>(0);        // n_t // n_0 = 0
    auto send_input_length_flag = std::make_unique<bool>(1);   // v_t // v_0 = 1

    // Prepare for file save operation
    std::ofstream uncompressed_file;
    std::ofstream compressed_file;
    uncompressed_file.open("original.bin", std::ios::out | std::ofstream::binary);
    compressed_file.open("compressed.bin", std::ios::out | std::ofstream::binary);

    auto input = std::make_unique<long int>(3333333333);

    std::deque<bool> input_vector;
    std::deque<bool> new_input_vector = number_to_deque_bool(input, input_vector_length);
    std::deque<bool> output_vector;
    std::deque<bool> new_output_vector;

    std::cout << "INPUT: " << std::endl;
    print_vector(new_input_vector);

    try{
        //uncompressed_file << bool_to_string(input_vector);
        input_vector.insert(input_vector.end(), new_input_vector.begin(), new_input_vector.end());
        new_output_vector = compressor.compress(
            new_input_vector, 
            robustness_level,
            new_mask_flag,
            send_mask_flag,
            uncompressed_flag,
            send_changes_flag,
            send_input_length_flag
        );
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));

        //compressed_file << bool_to_string(output_vector);

        std::cout << "OUTPUT: " << std::endl;
        print_vector(output_vector);

        new_mask_flag = std::make_unique<bool>(0);
        send_mask_flag = std::make_unique<bool>(1);
        uncompressed_flag = std::make_unique<bool>(0);
        send_changes_flag = std::make_unique<bool>(1);

        //uncompressed_file << bool_to_string(input_vector);
        input_vector.insert(input_vector.end(), new_input_vector.begin(), new_input_vector.end());
        new_output_vector = compressor.compress(
            new_input_vector, 
            robustness_level,
            new_mask_flag,
            send_mask_flag,
            uncompressed_flag,
            send_changes_flag,
            send_input_length_flag
        );
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));
        //compressed_file << bool_to_string(output_vector);
        print_vector(output_vector);

        new_mask_flag = std::make_unique<bool>(0);
        send_mask_flag = std::make_unique<bool>(1);
        uncompressed_flag = std::make_unique<bool>(0);
        send_changes_flag = std::make_unique<bool>(1);

        //uncompressed_file << bool_to_string(input_vector);
        input_vector.insert(input_vector.end(), new_input_vector.begin(), new_input_vector.end());
        new_output_vector = compressor.compress(
            new_input_vector, 
            robustness_level,
            new_mask_flag,
            send_mask_flag,
            uncompressed_flag,
            send_changes_flag,
            send_input_length_flag
        );
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));
        //compressed_file << bool_to_string(output_vector);
        print_vector(output_vector);

        new_mask_flag = std::make_unique<bool>(0);
        send_mask_flag = std::make_unique<bool>(1);
        uncompressed_flag = std::make_unique<bool>(0);
        send_changes_flag = std::make_unique<bool>(0);

        input = std::make_unique<long int>(3333333334);
        new_input_vector = number_to_deque_bool(input, input_vector_length);
        std::cout << "INPUT: " << std::endl;
        print_vector(new_input_vector);

        //uncompressed_file << bool_to_string(input_vector);
        input_vector.insert(input_vector.end(), new_input_vector.begin(), new_input_vector.end());
        new_output_vector = compressor.compress(
            new_input_vector, 
            robustness_level,
            new_mask_flag,
            send_mask_flag,
            uncompressed_flag,
            send_changes_flag,
            send_input_length_flag
        );
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));

        while(output_vector.size() % 8 != 0){
            output_vector.emplace_back(0);
        }
        while(input_vector.size() % 8 != 0){
            input_vector.emplace_back(0);
        }
        uncompressed_file << bool_to_string(input_vector);
        compressed_file << bool_to_string(output_vector);
        std::cout << "OUTPUT: " << std::endl;
        print_vector(output_vector);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    uncompressed_file.close();
    compressed_file.close();
}
