#include <cassert>
#include <iostream>

#include "matrices.hpp"

#define SHOW(...) \
    std::cout << #__VA_ARGS__ << " == " << __VA_ARGS__ << '\n'

int main()
{
try
{
    unsigned ax, ay, bx, by;

    std::cout << "Enter dimensions of matrix A: ";
    std::cin >> ax >> ay;
    matrices::matrix_t<double> A{ax, ay};
    std::cout << "Enter matrix elements of A:\n";
    A.scan();

    A[1][1] = 9;

    std::cout << "Enter dimensions of matrix B: ";
    std::cin >> bx >> by;
    matrices::matrix_t<double> B{bx, by};
    std::cout << "Enter matrix elements of B:\n";
    B.scan();

    matrices::decomposed_matrix C = B.decompose();
    std::cout << "C (= decomposed B):\n";
    C.dump();

    matrices::swap(A, B);
    std::cout << "A after swap:\n";
    A.dump();
    std::cout << "B after swap:\n";
    B.dump();

    return 0;
}
catch (matrices::matrix_square_required<double> exception)
{
    std::cout << exception.what() << "\n";
    exception.matrix.dump();
    return 0;
}
catch (matrices::matrix_out_of_range<double> exception)
{
    std::cout << exception.what() << " [" << exception.row << "][" << exception.col << "]\n";
    exception.matrix.dump();
    return 0;
}
}
