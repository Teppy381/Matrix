#pragma once

#include <cassert>
#include <iostream>
#include <cmath>
#include <type_traits>
#include <sstream>
#include <exception>

namespace matrices
{

bool is_zero(double x)
{
    if (std::abs(x) < __DBL_EPSILON__)
    {
        return true;
    }
    return false;
}

template <typename num_t>
struct matrix_out_of_range;

template <typename num_t>
struct matrix_square_required;

template <typename num_t>
struct decomposed_matrix;

template <typename num_t>
class matrix_t
{
    unsigned cols_, rows_; // columns and rows count
    num_t* storage;
    num_t** rows_arr;

    struct ProxyRow
    {
        num_t* row_;
        const unsigned row_n_;
        const unsigned cols_;
        const matrix_t& A_;

        ProxyRow(num_t* row, const unsigned row_n, const unsigned cols, const matrix_t& A) :
            row_{row}, row_n_{row_n}, cols_{cols}, A_{A}
        {} // constructor

        const num_t& operator[](unsigned col_n) const
        {
            if (col_n >= cols_)
            {
                throw matrix_out_of_range<num_t>{A_, row_n_, col_n, "In matrix cannot access element"};
            }
            return row_[col_n];
        }
        num_t& operator[](unsigned col_n)
        {
            if (col_n >= cols_)
            {
                throw matrix_out_of_range<num_t>{A_, row_n_, col_n, "In matrix cannot access element"};
            }
            return row_[col_n];
        }
    };

public:

    matrix_t(unsigned rows_a, unsigned cols_a) // 2 arguments constructor
    {
        cols_ = cols_a;
        rows_ = rows_a;

        storage = new num_t[cols_a * rows_a];
        rows_arr = new num_t*[rows_a];

        for (int i = 0; i != rows_a; ++i)
        {
            rows_arr[i] = storage + i * cols_a;
        }

        clear();
    }

    matrix_t(unsigned size) : matrix_t(size, size) // 1 argument constructor
    {}

    ~matrix_t() // destructor
    {
        // std::cout << "destructor\n";
        delete[] rows_arr;
        delete[] storage;
    }

    void swap(matrix_t& rhs) noexcept
    {
        std::swap(cols_, rhs.cols_);
        std::swap(rows_, rhs.rows_);
        std::swap(storage, rhs.storage);
        std::swap(rows_arr, rhs.rows_arr);
    }

    matrix_t(const matrix_t& rhs) // copy constructor
    {
        // std::cout << "copy constructor\n";
        matrix_t<num_t> tmp{rhs.rows_, rhs.cols_};
        for (int i = 0; i != tmp.rows_; ++i)
        {
            for (int j = 0; j != tmp.cols_; ++j)
            {
                tmp[i][j] = rhs[i][j];
            }
        }
        swap(tmp);
        tmp.rows_arr = NULL;
        tmp.storage = NULL;
    }

    matrix_t(matrix_t&& rhs) noexcept // move constructor
    {
        // std::cout << "move constructor\n";
        swap(rhs);
    }

    matrix_t& operator=(matrix_t&& rhs) noexcept // move assign
    {
        // std::cout << "move assign\n";
        if (this != &rhs)
        {
            swap(rhs);
        }
        return *this;
    }


    matrix_t& operator=(const matrix_t& rhs) // copy assign
    {
        // std::cout << "copy assign\n";
        if (this == &rhs)
        {
            return *this;
        }
        matrix_t tmp(rhs);

        swap(tmp);
        return *this;
    }

    template <typename new_num_t>
    matrix_t<new_num_t> copy() const // copy into different type of matrix
    {
        matrix_t<new_num_t> new_matrix{rows_, cols_};
        for (int i = 0; i != rows_; ++i)
        {
            for (int j = 0; j != cols_; ++j)
            {
                new_matrix[i][j] = static_cast<new_num_t>((*this)[i][j]);
            }
        }
        return new_matrix;
    }

    ProxyRow operator[](unsigned row_n)
    {
        if (row_n >= rows_)
        {
            throw matrix_out_of_range<num_t>{*this, row_n, 0, "In matrix cannot access row"};
        }
        ProxyRow row{rows_arr[row_n], row_n, cols_, *this};
        return row;
    }

    const ProxyRow operator[](unsigned row_n) const
    {
        if (row_n >= rows_)
        {
            throw matrix_out_of_range<num_t>{*this, row_n, 0, "In matrix cannot access row"};
        }

        const ProxyRow row{rows_arr[row_n], row_n, cols_, *this};
        return row;
    }

    // getters
    unsigned cols() const
    {
        return cols_;
    }
    unsigned rows() const
    {
        return rows_;
    }


    void swap_rows(unsigned row_first, unsigned row_second)
    {
        if (row_first == row_second)
        { return; }
        std::swap(rows_arr[row_first], rows_arr[row_second]);
    }

    void clear()
    {
        for (unsigned i = 0, size = cols_ * rows_; i != size; ++i)
        {
            storage[i] = 0;
        }
    }

    void make_identity()
    {
        if (rows_ != cols_)
        {
            throw matrix_square_required(*this, "Cannot make identity matrix from a non-square matrix");
        }
        clear();
        for (unsigned i = 0; i < cols_; ++i)
        {
            (*this)[i][i] = 1;
        }
    }

    void dump_row(unsigned row) const
    {
        for (int i = 0; i != cols_; ++i)
        {
            std::cout << *(rows_arr[row] + i) << " ";
        }
        std::cout << "\n";
    }

    void dump() const
    {
        std::cout << "{" << rows_ << "}{" << cols_ <<"}\n";
        for (int i = 0; i != rows_; ++i)
        {
            dump_row(i);
        }
        std::cout << "\n";
    }

    void scan()
    {
        for (unsigned i = 0; i < rows_; ++i)
        {
            for (unsigned j = 0; j < cols_; ++j)
            {
                std::cin >> (*this)[i][j];
                if (!std::cin.good())
                { return; }
            }
        }
    }

    void scan(const char* str)
    {
        std::streambuf* orig_stream = std::cin.rdbuf();
        std::istringstream input(str);
        std::cin.rdbuf(input.rdbuf());

        scan();

        std::cin.rdbuf(orig_stream);
    }


    decomposed_matrix<num_t> decompose() const;

    num_t calc_determinant() const
    {
        if (rows_ != cols_)
        {
            throw matrix_square_required(*this, "Cannot calculate determinant for a non-square matrix");
        }

        matrix_t<double> U = this->copy<double>();
        int pivoting_det = 1;

        unsigned h = 0; // pivot row
        unsigned k = 0; // pivot column
        while (h < rows_ && k < cols_)
        {
            /* Find the k-th pivot: */
            unsigned i_max = 0;
            double max = 0;
            for (unsigned i = h; i < rows_; ++i)
            {
                double value = std::abs(U[i][k]);
                if (value > max)
                {
                    max = value;
                    i_max = i;
                }
            }

            if (is_zero(max))
            {
                /* No pivot in this column, pass to next column */
                k += 1;
                continue;
            }

            U.swap_rows(h, i_max);
            if (h != i_max)
            {
                pivoting_det = -pivoting_det;
            }
            /* Do for all rows below pivot: */
            for (unsigned i = h + 1; i < rows_; ++i)
            {
                double f = U[i][k] / U[h][k];

                U[i][k] = 0;

                for (unsigned j = k + 1; j < cols_; ++j)
                {
                    U[i][j] -= U[h][j] * f;
                }
            }
            /* Increase pivot row and column */
            h += 1;
            k += 1;
        }

        double det = pivoting_det;
        for (unsigned i = 0; i < rows_; ++i)
        {
            det *= U[i][i];
        }

        if (std::is_integral_v<num_t>)
        {
            det = round(det);
        }

        if (det == 0) // to get rid of -0
        {
            return 0;
        }
        return (num_t) det;
    }


}; // end of matrix_t

template <typename num_t>
struct decomposed_matrix // such decomposition of matrix M that P*M == L*U
{
    matrix_t<num_t> M;
    int pivoting_det;
    matrix_t<int>    P;
    matrix_t<double> U;
    matrix_t<double> L;
    num_t determinant;

    decomposed_matrix(const matrix_t<num_t>& A) : M{A}, P{A.cols()}, U{A.cols()}, L{A.cols()} // constructor
    {
        // assert(A.cols() == A.rows());
        if (A.cols() != A.rows())
        {
            std::cout << "Cannot make LUP decomposition for a non-square matrix\n";
            A.dump();
            return;
        }

        U = A.template copy<double>();
        L.make_identity();
        P.make_identity();
        pivoting_det = 1;

        unsigned h = 0; // pivot row
        unsigned k = 0; // pivot column
        while (h < U.rows() && k < U.cols())
        {
            /* Find the k-th pivot: */
            unsigned i_max = 0;
            double max = 0;
            for (unsigned i = h; i < U.rows(); ++i)
            {
                double value = std::abs(U[i][k]);
                if(value > max)
                {
                    max = value;
                    i_max = i;
                }
            }

            if (is_zero(max))
            {
                /* No pivot in this column, pass to next column */
                k += 1;
                continue;
            }

            U.swap_rows(h, i_max);
            P.swap_rows(h, i_max);
            if (h != i_max)
            {
                pivoting_det = -pivoting_det;
            }
            /* Do for all rows below pivot: */
            for (unsigned i = h + 1; i < U.rows(); ++i)
            {
                double f = U[i][k] / U[h][k];

                /* Fill with zeros the L part of pivot column: */
                U[i][k] = 0;

                /* Do for all remaining elements in current row: */
                for (unsigned j = k + 1; j < U.cols(); ++j)
                {
                    U[i][j] -= U[h][j] * f;
                }
                L[i][h] = f;
            }
            /* Increase pivot row and column */
            h += 1;
            k += 1;
        }

        double det = pivoting_det;
        assert(U.cols() == U.rows());
        for (unsigned i = 0; i < U.cols(); ++i)
        {
            det *= U[i][i];
        }

        if (std::is_integral_v<num_t>)
        {
            det = round(det);
        }
        if (det == 0) // to get rid of -0
        {
            det = 0;
        }
        determinant = (num_t) det;
    }

    void dump() const
    {
        std::cout << "M";
        M.dump();
        std::cout << "P";
        P.dump();
        std::cout << "U";
        U.dump();
        std::cout << "L";
        L.dump();
    }
}; // end of decomposed_matrix

template <typename num_t>
decomposed_matrix<num_t> matrix_t<num_t>::decompose() const
{
    if (rows_ != cols_)
    {
        throw matrix_square_required(*this, "Cannot make LUP decomposition for a non-square matrix");
    }
    decomposed_matrix decomp{*this};
    return decomp;
}

template <typename num_t>
void swap(matrix_t<num_t>& A, matrix_t<num_t>& B) noexcept
{
    A.swap(B);
}

template <typename num_t>
struct matrix_out_of_range : public std::out_of_range
{
    const matrix_t<num_t> matrix;
    unsigned row;
    unsigned col;

    matrix_out_of_range(matrix_t<num_t> matrix_arg, unsigned row_n, unsigned col_n, const char* what_arg)
    : matrix{matrix_arg}, row{row_n}, col{col_n}, std::out_of_range{what_arg}
    {}
};

template <typename num_t>
struct matrix_square_required : public std::runtime_error
{
    const matrix_t<num_t> matrix;

    matrix_square_required(matrix_t<num_t> matrix_arg, const char* what_arg)
    : matrix{matrix_arg}, std::runtime_error{what_arg}
    {}
};

} // end of namespace matrices
