#include <cassert>
#include <iostream>

#include "matrices.hpp"

int main()
{
    unsigned size;

    std::cout << "Enter size: ";
    std::cin >> size;

    matrices::matrix_t my_matrix = matrices::matrix_t{size};

    for (int i = 0; i != size; ++i)
    {
        my_matrix[i][i] = 5;
    }
    my_matrix.dump();

    return 0;
}
