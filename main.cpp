#include <cassert>
#include <iostream>

#include "matrices.hpp"

int main()
{
    unsigned x, y;

    // std::cout << "Enter matrix dimensions: ";
    std::cin >> x;

    matrices::matrix_t my_matrix = matrices::matrix_t{x};

    // std::cout << "Enter matrix elements:\n";
    my_matrix.scan_from_stdin();

    // std::cout << "Your matrix is:\n";
    // my_matrix.dump();

    // std::cout << "Determinant is: " << my_matrix.calc_determinant() << "\n";
    // my_matrix.dump_decomposition();

    std::cout << my_matrix.calc_determinant() << "\n";

    return 0;
}
