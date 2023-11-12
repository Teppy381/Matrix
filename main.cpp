#include <cassert>
#include <iostream>

#include "matrices.hpp"

#define SHOW(...) \
    std::cout << #__VA_ARGS__ << " == " << __VA_ARGS__ << '\n'

int main()
{
    unsigned x, y;

    std::cout << "Enter matrix size: ";
    std::cin >> x >> y;

    matrices::matrix_t<double> A = matrices::matrix_t<double>{x, y};

    std::cout << "Enter matrix elements:\n";
    A.scan_from_stdin();

    // std::cout << "Your <double> matrix is:\n";
    // A.dump();

    matrices::matrix_t<int> B = A.copy<int>();
    // std::cout << "Your <int> matrix is:\n";
    // B.dump();

    // SHOW(B.calc_determinant());
    // SHOW(A.calc_determinant());

    matrices::decomposed_matrix A_dec = A.decompose();

    SHOW(A_dec.determinant);
    SHOW(A.calc_determinant());

    return 0;
}
