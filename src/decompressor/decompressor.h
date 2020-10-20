#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <deque>
#include <memory>

#include "../utils/utils.h"

namespace pocketplus {
namespace decompressor {

class PocketPlusDecompressor{
    // Private functions
    // Constants
    // Define inputs and parameters
    // Vectors

    public:
        PocketPlusDecompressor(std::unique_ptr<unsigned int>& vector_length){
        }
        // Public functions
        std::deque<bool> decompress(const std::deque<bool>& input);
};

};
};

#endif