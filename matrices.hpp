#pragma once

#include <cassert>
#include <iostream>
#include <cmath>
#include <type_traits>
#include <sstream>

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
struct decomposed_matrix;

template <typename num_t>
class matrix_t
{
    unsigned cols_, rows_; // columns and rows count
    num_t* storage;
    num_t** rows_arr;

    struct ProxyRow
    {
        ProxyRow(num_t* row, const unsigned row_n, const unsigned cols, const matrix_t* A):
            row_{row}, row_n_{row_n}, cols_{cols}, A_{A}
        {}
        num_t* row_;
        const unsigned row_n_;
        const unsigned cols_;
        const matrix_t* A_;

        const num_t& operator[](int col_n) const
        {
            if (col_n >= cols_)
            {
                std::cout << "Cannot access [" << row_n_ << "][" << col_n << "] element of matrix\n";
                A_->dump();
                exit(1);
            }
            return row_[col_n];
        }
        num_t& operator[](int col_n)
        {
            if (col_n >= cols_)
            {
                std::cout << "Cannot access [" << row_n_ << "][" << col_n << "] element of matrix\n";
                A_->dump();
                exit(1);
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

    matrix_t(const matrix_t& rhs) // copy constructor
    {
        // std::cout << "copy constructor\n";
        cols_ = rhs.cols_;
        rows_ = rhs.rows_;

        storage = new num_t [rhs.cols_ * rhs.rows_];
        for (int i = 0, size = rhs.cols_ * rhs.rows_; i != size; ++i)
        {
            storage[i] = rhs.storage[i];
        }

        rows_arr = new num_t* [rhs.rows_];
        for (int i = 0; i != rhs.rows_; ++i)
        {
            rows_arr[i] = storage + (rhs.rows_arr[i] - rhs.storage);
        }
    }

    matrix_t(matrix_t&& rhs) // move constructor
    {
        // std::cout << "move constructor\n";
        cols_ = rhs.cols_;
        rows_ = rhs.rows_;
        storage = rhs.storage;
        rows_arr = rhs.rows_arr;

        rhs.rows_ = 0;
        rhs.cols_ = 0;
        rhs.storage = nullptr;
        rhs.rows_arr = nullptr;
    }

    ~matrix_t() // destructor
    {
        // std::cout << "destructor\n";
        delete[] rows_arr;
        delete[] storage;
    }

    matrix_t& operator=(matrix_t&& rhs) // move assign
    {
        // std::cout << "move assign\n";
        if (this == &rhs)
        {
            return *this;
        }

        delete[] rows_arr;
        delete[] storage;

        cols_ = rhs.cols_;
        rows_ = rhs.rows_;
        storage = rhs.storage;
        rows_arr = rhs.rows_arr;

        rhs.rows_ = 0;
        rhs.cols_ = 0;
        rhs.storage = nullptr;
        rhs.rows_arr = nullptr;
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

        std::swap(rows_, tmp.rows_);
        std::swap(cols_, tmp.cols_);
        std::swap(storage, tmp.storage);
        std::swap(rows_arr, tmp.rows_arr);

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
            std::cout << "Cannot access [" << row_n << "] row of matrix:\nHere is the dump:\n";
            dump();
            exit(1);
        }
        ProxyRow row{rows_arr[row_n], row_n, cols_, this};
        return row;
    }

    const ProxyRow operator[](unsigned row_n) const
    {
        if (row_n >= rows_)
        {
            std::cout << "Cannot access [" << row_n << "] row of matrix:\nHere is the dump:\n";
            dump();
            exit(1);
        }

        const ProxyRow row{rows_arr[row_n], row_n, cols_, this};
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
        if (row_first >= rows_ || row_second >= rows_)
        {
            std::cout << "Cannot access [" << row_first << "] or [" << row_second << "] row of matrix\n";
            exit(1);
        }

        if (row_first == row_second)
        { return; }
        num_t* temp = rows_arr[row_first];
        rows_arr[row_first] = rows_arr[row_second];
        rows_arr[row_second] = temp;
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
            std::cout << "Cannot make identity matrix from a non-quadratic matrix\n";
            exit(1);
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
            std::cout << "Cannot calculate determinant for a non-square matrix\n";
            exit(1);
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
        std::cout << "\nM";
        M.dump();
        std::cout << "P";
        P.dump();
        std::cout << "U";
        U.dump();
        std::cout << "L";
        L.dump();
    }
};

template <typename num_t>
decomposed_matrix<num_t> matrix_t<num_t>::decompose() const
{
    if (rows_ != cols_)
    {
        std::cout << "Cannot make LUP decomposition for a non-square matrix\n";
        exit(1);
    }
    decomposed_matrix decomp{*this};
    return decomp;
}

} // end of namespace matrices
