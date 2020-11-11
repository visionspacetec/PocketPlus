#include <iterator>

#include "compressor.h"
#include "decompressor.h"
#include "utils.h"

int main(int argc, char* argv[]){

    std::cout << "Welcome to PocketPlus in c++" << std::endl;

    auto input_vector_length = std::make_unique<unsigned int>(16); // In bits

    pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length);

    std::deque<unsigned int> robustness_level = {1, 1, 1, 1, 1, 1, 1}; // R_t
    std::deque<bool> new_mask_flag            = {1, 0, 0, 0, 0, 0, 0}; // p_t
    std::deque<bool> send_mask_flag           = {1, 1, 0, 0, 0, 0, 0}; // f_t // if t <= R_t then f_t = 1
    std::deque<bool> uncompressed_flag        = {1, 1, 0, 0, 0, 0, 0}; // r_t // if t <= R_t then r_t = 1
    std::deque<long int> input = {
        3333333333, // 1
        3333333333, // 2
        3333333333, // 3
        3333333334, // 4
        3333333334, // 5
        3333333334, // 6
        3333333737  // 7
    };

    try{
        std::deque<std::deque<bool>> input_vector;
        std::deque<bool> total_input_vector;
        std::deque<bool> total_output_vector;
        std::deque<bool> new_output_vector;
        for(auto in: input){
            input_vector.emplace_back(pocketplus::utils::number_to_deque_bool(in, *input_vector_length));
        }
        auto counter = std::make_unique<unsigned int>(1);
        for(auto in: input_vector){
            std::cout << "Compressing data frame ###" << *counter << std::endl;
            std::cout << "INPUT: " << std::endl;
            pocketplus::utils::print_vector(in);

            total_input_vector.insert(total_input_vector.end(), in.begin(), in.end());
            new_output_vector = compressor.compress(
                in, 
                robustness_level.front(),
                new_mask_flag.front(),
                send_mask_flag.front(),
                uncompressed_flag.front()
            );
            pocketplus::utils::zero_stuffing(new_output_vector);
            std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(total_output_vector));
            
            robustness_level.pop_front();
            new_mask_flag.pop_front();
            send_mask_flag.pop_front();
            uncompressed_flag.pop_front();

            *counter += 1;
        }

        std::cout << "OUTPUT: " << std::endl;
        pocketplus::utils::print_vector(total_output_vector);

        // Save the total input and output to separate files
        pocketplus::utils::write_bool_deque_to_file("original.bin", total_input_vector);
        pocketplus::utils::write_bool_deque_to_file("compressed.bin", total_output_vector);

        // Compression ratio
        auto compression_ratio = std::make_unique<float>((float)total_input_vector.size() / (float)total_output_vector.size());
        std::cout << "Input vector size: " << total_input_vector.size() << " bits" << std::endl;
        std::cout << "Output vector size: " << total_output_vector.size() << " bits" << std::endl;
        std::cout << "Compression ratio: " << *compression_ratio << std::endl;

        // Decompression
        std::cout << "############# DECOMPRESSION #############" << std::endl;
        std::cout << "INPUT: " << std::endl;
        auto read_compressed = pocketplus::utils::read_bool_deque_from_file("compressed.bin");
        pocketplus::utils::print_vector(read_compressed);

        pocketplus::decompressor::PocketPlusDecompressor decompressor(input_vector_length);

        auto i = std::make_unique<unsigned int>(1);
        std::deque<bool> data;
        auto data_int = std::make_unique<long int>(0);
        while(read_compressed.size() > 0){
            data = decompressor.decompress(read_compressed);
            *data_int = pocketplus::utils::deque_bool_to_number(data);
            std::cout << "Decompressed #" << *i << ": " << *data_int << std::endl;
            *i += 1;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}