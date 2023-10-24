#pragma once

#include <cassert>
#include <iostream>
#include <cmath>
// #include <iterator>
// #include <list>
// #include <unordered_map>
// #include <vector>

namespace matrices
{

class matrix_t
{
    unsigned cols, rows; // columns and rows count
    double* storage;
    double** rows_arr;

    bool is_decomposed; // such decomposition of matrix A that P*A == L*U
    matrix_t* P;
    matrix_t* L;
    matrix_t* U;
    double determinant;

    struct ProxyRow
    {
        double* row;
        unsigned row_n;
        unsigned cols;
        matrix_t* A;

        const double& operator[](int col_n) const
        {
            // std::cout << "accessing [" << row_n << "][" << col_n << "]\n";
            // A->dump();
            if (col_n >= cols)
            {
                std::cout << "Cannot access [" << row_n << "][" << col_n << "] element of matrix\n";
                A->dump();
                exit(1);
            }
            return row[col_n];
        }
        double& operator[](int col_n)
        {
            // std::cout << "accessing [" << row_n << "][" << col_n << "]\n";
            // A->dump();
            if (col_n >= cols)
            {
                std::cout << "Cannot access [" << row_n << "][" << col_n << "] element of matrix\n";
                A->dump();
                exit(1);
            }
            return row[col_n];
        }
    };

public:

    matrix_t(unsigned rows_a, unsigned cols_a) // 2 arguments constructor
    {
        cols = cols_a;
        rows = rows_a;

        storage = new double[cols_a * rows_a];
        rows_arr = new double*[rows_a];

        is_decomposed = false;
        P = nullptr;
        U = nullptr;
        L = nullptr;
        determinant = NAN;

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
        cols = rhs.cols;
        rows = rhs.rows;

        storage = new double[rhs.cols * rhs.rows];
        for (int i = 0, size = rhs.cols * rhs.rows; i != size; ++i)
        {
            storage[i] = rhs.storage[i];
        }

        rows_arr = new double*[rhs.rows];
        for (int i = 0; i != rhs.rows; ++i)
        {
            rows_arr[i] = storage + (rhs.rows_arr[i] - rhs.storage);
        }

        P = nullptr;
        U = nullptr;
        L = nullptr;
        determinant = NAN;

        is_decomposed = rhs.is_decomposed;
        if (is_decomposed)
        {
            assert(rhs.P != nullptr && rhs.U != nullptr && rhs.L != nullptr);
            P = new matrix_t{rhs.P->rows, rhs.P->cols};
            U = new matrix_t{rhs.U->rows, rhs.U->cols};
            L = new matrix_t{rhs.L->rows, rhs.L->cols};
            *P = *(rhs.P);
            *U = *(rhs.U);
            *L = *(rhs.L);
            determinant = rhs.determinant;
        }
    }

    matrix_t(matrix_t&& rhs) // move constructor
    {
        // std::cout << "move constructor\n";
        cols = rhs.cols;
        rows = rhs.rows;
        storage = rhs.storage;
        rows_arr = rhs.rows_arr;
        is_decomposed = rhs.is_decomposed;
        P = rhs.P;
        U = rhs.U;
        L = rhs.L;
        determinant = rhs.determinant;

        rhs.rows = 0;
        rhs.cols = 0;
        rhs.storage = nullptr;
        rhs.rows_arr = nullptr;
        rhs.is_decomposed = false;
        rhs.P = nullptr;
        rhs.U = nullptr;
        rhs.L = nullptr;
    }

    ~matrix_t() // destructor
    {
        // std::cout << "destructor\n";
        delete P;
        delete U;
        delete L;
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

        delete P;
        delete U;
        delete L;
        delete[] rows_arr;
        delete[] storage;

        cols = rhs.cols;
        rows = rhs.rows;
        storage = rhs.storage;
        rows_arr = rhs.rows_arr;
        is_decomposed = rhs.is_decomposed;
        P = rhs.P;
        U = rhs.U;
        L = rhs.L;
        determinant = rhs.determinant;

        rhs.rows = 0;
        rhs.cols = 0;
        rhs.storage = nullptr;
        rhs.rows_arr = nullptr;
        rhs.is_decomposed = false;
        rhs.P = nullptr;
        rhs.U = nullptr;
        rhs.L = nullptr;
        return *this;
    }

    matrix_t& operator=(const matrix_t& rhs) // copy assign
    {
        // std::cout << "copy assign\n";
        if (this == &rhs)
        {
            return *this;
        }
        if (cols != rhs.cols ||  rows != rhs.rows)
        {
            cols = rhs.cols;
            rows = rhs.rows;

            delete[] rows_arr;
            delete[] storage;
            storage = new double[rhs.cols * rhs.rows];
            rows_arr = new double*[rhs.rows];
        }

        for (int i = 0; i != rhs.rows; ++i)
        {
            rows_arr[i] = storage + (rhs.rows_arr[i] - rhs.storage);
        }

        for (int i = 0, size = rhs.cols * rhs.rows; i != size; ++i)
        {
            storage[i] = rhs.storage[i];
        }

        delete P;
        delete U;
        delete L;
        P = nullptr;
        U = nullptr;
        L = nullptr;
        determinant = NAN;

        is_decomposed = rhs.is_decomposed;
        if (is_decomposed)
        {
            assert(rhs.P != nullptr && rhs.U != nullptr && rhs.L != nullptr);
            P = new matrix_t{rhs.P->rows, rhs.P->cols};
            U = new matrix_t{rhs.U->rows, rhs.U->cols};
            L = new matrix_t{rhs.L->rows, rhs.L->cols};
            *P = *(rhs.P);
            *U = *(rhs.U);
            *L = *(rhs.L);
            determinant = rhs.determinant;
        }
        return *this;
    }


    ProxyRow operator[](unsigned row_n)
    {
        if (row_n >= rows)
        {
            std::cout << "Cannot access [" << row_n << "] row of matrix:\nHere is the dump:\n";
            dump();
            exit(1);
        }

        ProxyRow row;
        row.cols = cols;
        row.row_n = row_n;
        row.row = rows_arr[row_n];
        row.A = this;
        return row;
    }


    void swap_rows(unsigned row_first, unsigned row_second)
    {
        if (row_first >= rows || row_second >= rows)
        {
            std::cout << "Cannot access [" << row_first << "] or [" << row_second << "] row of matrix\n";
            exit(1);
        }

        if (row_first == row_second)
        { return; }
        double* temp = rows_arr[row_first];
        rows_arr[row_first] = rows_arr[row_second];
        rows_arr[row_second] = temp;
    }

    void clear()
    {
        for (unsigned i = 0, size = cols * rows; i != size; ++i)
        {
            storage[i] = 0;
        }
    }

    void make_identity()
    {
        if (rows != cols)
        {
            std::cout << "Cannot make identity matrix from a non-quadratic matrix\n";
            exit(1);
        }
        clear();
        for (unsigned i = 0; i < cols; ++i)
        {
            (*this)[i][i] = 1;
        }
    }

    void dump_row(unsigned row) const
    {
        for (int i = 0; i != cols; ++i)
        {
            std::cout << *(rows_arr[row] + i) << " ";
        }
        std::cout << "\n";
    }

    void dump() const
    {
        for (int i = 0; i != rows; ++i)
        {
            dump_row(i);
        }
        std::cout << "\n";
    }

    void dump_decomposition()
    {
        if (!is_decomposed)
        {
            std::cout << "This matrix has no decomposition\n";
            exit(1);
        }
        std::cout << "P:\n";
        P->dump();
        std::cout << "U:\n";
        U->dump();
        std::cout << "L:\n";
        L->dump();
    }

    void scan_from_stdin()
    {
        for (unsigned i = 0; i < rows; ++i)
        {
            for (unsigned j = 0; j < cols; ++j)
            {
                std::cin >> (*this)[i][j];
                if (!std::cin.good())
                { return; }
            }
        }
    }

// private:
    bool is_zero(double x) const
    {
        if (std::abs(x) < __DBL_EPSILON__)
        {
            return true;
        }
        return false;
    }


    void decompose()
    {
        if (rows != cols)
        {
            std::cout << "Cannot make LUP decomposition for a non-quadratic matrix\n";
            exit(1);
        }
        U = new matrix_t{rows};
        *U = *this;

        L = new matrix_t{rows};
        L->make_identity();

        P = new matrix_t{rows};
        P->make_identity();
        P->determinant = 1;


        unsigned h = 0; // pivot row
        unsigned k = 0; // pivot column
        while (h < rows && k < cols)
        {
            /* Find the k-th pivot: */
            // std::cout << "\e[35m";
            // std::cout << "h = " << h << "\nk = " << k << "\nrows = " << rows << "\ncols = " << cols << "\n";

            unsigned i_max = 0;
            double max = 0;
            for (unsigned i = h; i < rows; ++i)
            {
                double value = std::abs((*U)[i][k]);
                if(value > max)
                {
                    max = value;
                    i_max = i;
                }
            }
            // std::cout << "\e[0m";
            if (is_zero(max))
            {
                /* No pivot in this column, pass to next column */
                k += 1;
            }
            else
            {
                U->swap_rows(h, i_max);
                P->swap_rows(h, i_max);
                if (h != i_max)
                {
                    P->determinant *= -1;
                }
                /* Do for all rows below pivot: */
                for (unsigned i = h + 1; i < rows; ++i)
                {
                    // std::cout << "\e[31m"; // RED
                    double f = (*U)[i][k] / (*U)[h][k];

                    // std::cout << "f = " << f << "\n";
                    /* Fill with zeros the L part of pivot column: */
                    // std::cout << "\e[32m"; // GREEN
                    (*U)[i][k] = 0;
                    /* Do for all remaining elements in current row: */
                    // std::cout << "\e[34m"; // BLUE
                    for (unsigned j = k + 1; j < cols; ++j)
                    {
                        (*U)[i][j] -= (*U)[h][j] * f;
                    }
                    (*L)[i][h] = f;
                    // std::cout << "\e[0m";
                }
                /* Increase pivot row and column */
                h += 1;
                k += 1;
            }
        }
        is_decomposed = true;
        return;
    }

    double calc_determinant()
    {
        if (determinant == determinant) // determinant != NAN
        {
            return determinant;
        }

        if (!is_decomposed)
        {
            decompose();
        }
        assert(P != nullptr && U != nullptr && L != nullptr);
        assert(cols == rows);

        double det = P->determinant;

        assert(U->cols == U->rows);
        for (unsigned i = 0; i < U->cols; ++i)
        {
            det *= (*U)[i][i];
        }
        if (det == 0) // to get rid of -0
        {
            determinant = 0;
            return 0;
        }
        determinant = det;
        return det;
    }



}; // end of matrix_t

} // end of namespace matrices
