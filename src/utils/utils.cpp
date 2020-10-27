#include "utils.h"

using namespace pocketplus::utils;

// Helper function for file saving
void pocketplus::utils::zero_stuffing(std::deque<bool>& in){
    while(in.size() % 8 != 0){
        in.emplace_back(0);
    }
}

// Prints a boolean deque
void pocketplus::utils::print_vector(const std::deque<bool>& in){
    for(auto i: in){
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

// Converts a long integer to a size n boolean vector
std::deque<bool> pocketplus::utils::number_to_deque_bool(std::unique_ptr<long int>& input, std::unique_ptr<unsigned int>& length){
    std::deque<bool> out;
    for(unsigned int i = 0; i < *length; i++){
        out.emplace_front((*input >> i) & 1);
    }
    return out;
}

// Converts a boolean deque to long int
long int pocketplus::utils::deque_bool_to_number(const std::deque<bool>& input){
    auto output = std::make_unique<long int>(0);
    auto bit_shift = std::make_unique<unsigned int>(0);

    auto one = std::make_unique<long int>(1);
    for(auto it = input.rbegin(); it < input.rend(); it++, *bit_shift += 1){
        if(*it){
            *output |= *one << *bit_shift;
        }
    }
    return *output;
}

// Helper function for bool_to_string
std::size_t pocketplus::utils::divide_up(std::size_t dividend, std::size_t divisor){
    return (dividend + divisor - 1) / divisor;
}

// Converts a boolean vector to a string
std::string pocketplus::utils::bool_to_string(std::deque<bool> const& boolvector){
    std::string ret(pocketplus::utils::divide_up(boolvector.size(), 8), 0);
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

// Saves a boolean deque to file
void pocketplus::utils::write_bool_deque_to_file(const std::string& file_name, const std::deque<bool>& in){
    std::ofstream output_file;
    output_file.open(file_name, std::ios::out | std::ofstream::binary);
    output_file << pocketplus::utils::bool_to_string(in);
    output_file.close();
}

// Reads as boolean deque from file and returns it
std::deque<bool> pocketplus::utils::read_bool_deque_from_file(const std::string& file_name){
    std::ifstream input_file;
    input_file.open(file_name, std::ios::in | std::ifstream::binary);
    std::deque<char> buffer_char;
    std::copy(
        std::istream_iterator<unsigned char>(input_file),
        std::istream_iterator<unsigned char>(),
        std::front_inserter(buffer_char));
    std::deque<bool> buffer_bool;
    for(auto byte: buffer_char){
        for(unsigned int i = 0; i < 8; i++){
            buffer_bool.emplace_front(((unsigned char)byte >> i) & 1);
        }
    }
    return buffer_bool;
}

// Pops n elements from the front of a boolean deque
void pocketplus::utils::pop_n_from_front(std::deque<bool>& in, unsigned int n){
    for(auto i = 0; i < n; i++){
        in.pop_front();
    }
}