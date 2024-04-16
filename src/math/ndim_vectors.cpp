#include "ndim_vectors.hpp"

std::vector<int> create_md_array(const std::vector<int> & dimensions)
{
    int size = std::accumulate(dimensions.begin(), 
        dimensions.end(), 1, 
        std::multiplies<int>());
    return std::vector<int>(size);
}