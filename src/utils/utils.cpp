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