#include <fstream>
#include <iterator>

#include "./compressor/compressor.h"
#include "./decompressor/decompressor.h"
#include "./utils/utils.h"

int main(int argc, char* argv[]){

    std::cout << "Welcome to PocketPlus in c++" << std::endl;

    auto input_vector_length = std::make_unique<unsigned int>(32); // In bits

    pocketplus::compressor::PocketPlusCompressor compressor(input_vector_length);

    auto robustness_level = std::make_unique<unsigned int>(1); // R_t
    auto new_mask_flag = std::make_unique<bool>(1);            // p_t
    auto send_mask_flag = std::make_unique<bool>(0);           // f_t // f_0 = 0
    auto uncompressed_flag = std::make_unique<bool>(1);        // r_t // if n_t == 0 -> r_t = 1 --> r_0 = 1
    auto send_changes_flag = std::make_unique<bool>(0);        // n_t // n_0 = 0
    auto send_input_length_flag = std::make_unique<bool>(1);   // v_t // v_0 = 1

    auto input = std::make_unique<long int>(3333333333);

    std::deque<bool> input_vector;
    std::deque<bool> new_input_vector = pocketplus::utils::number_to_deque_bool(input, input_vector_length);
    std::deque<bool> output_vector;
    std::deque<bool> new_output_vector;

    std::cout << "INPUT: " << std::endl;
    pocketplus::utils::print_vector(new_input_vector);

    try{
        // 1
        std::cout << "#### 1" << std::endl;
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
        pocketplus::utils::zero_stuffing(new_output_vector);
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));

        std::cout << "OUTPUT: " << std::endl;
        pocketplus::utils::print_vector(output_vector);

        // 2
        std::cout << "#### 2" << std::endl;
        new_mask_flag = std::make_unique<bool>(0);
        send_mask_flag = std::make_unique<bool>(1);
        uncompressed_flag = std::make_unique<bool>(0);
        send_changes_flag = std::make_unique<bool>(1);

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
        pocketplus::utils::zero_stuffing(new_output_vector);
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));
        pocketplus::utils::print_vector(output_vector);

        // 3
        std::cout << "#### 3" << std::endl;
        //new_mask_flag = std::make_unique<bool>(0);
        //send_mask_flag = std::make_unique<bool>(0);
        //uncompressed_flag = std::make_unique<bool>(0);
        //send_changes_flag = std::make_unique<bool>(0);
        //send_input_length_flag = std::make_unique<bool>(0);

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
        pocketplus::utils::zero_stuffing(new_output_vector);
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));
        pocketplus::utils::print_vector(output_vector);

        // 4
        std::cout << "#### 4" << std::endl;
        new_mask_flag = std::make_unique<bool>(0);
        send_mask_flag = std::make_unique<bool>(1);
        uncompressed_flag = std::make_unique<bool>(0);
        send_changes_flag = std::make_unique<bool>(0);

        input = std::make_unique<long int>(3333333334);
        new_input_vector = pocketplus::utils::number_to_deque_bool(input, input_vector_length);
        std::cout << "INPUT: " << std::endl;
        pocketplus::utils::print_vector(new_input_vector);

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
        pocketplus::utils::zero_stuffing(new_output_vector);
        std::move(new_output_vector.begin(), new_output_vector.end(), std::back_inserter(output_vector));

        pocketplus::utils::zero_stuffing(input_vector);
        std::cout << "OUTPUT: " << std::endl;
        pocketplus::utils::print_vector(output_vector);

        // Save the total input and output to separate files
        pocketplus::utils::write_bool_deque_to_file("original.bin", input_vector);
        pocketplus::utils::write_bool_deque_to_file("compressed.bin", output_vector);

        std::cout << "############# DECOMPRESSION #############" << std::endl;
        std::cout << "INPUT: " << std::endl;
        auto read_compressed = pocketplus::utils::read_bool_deque_from_file("compressed.bin");
        pocketplus::utils::print_vector(read_compressed);

        pocketplus::decompressor::PocketPlusDecompressor decompressor(input_vector_length);

        auto first_data = decompressor.decompress(read_compressed);
        pocketplus::utils::print_vector(first_data);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
