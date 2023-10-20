#pragma once

#include <cassert>
#include <iostream>
// #include <iterator>
// #include <list>
// #include <unordered_map>
// #include <vector>


namespace matrices
{

class matrix_t
{
    unsigned cols, rows;
    double* storage;
    double** rows_arr;

    struct ProxyRow
    {
        double* row;
        unsigned row_n;
        unsigned cols;

        const double& operator[](int col_n) const
        {
            if (col_n >= cols)
            {
                std::cout << "Cannot access [" << row_n << "][" << col_n << "] element of matrix\n";
                // wanna dump();
                exit(1);
            }
            return row[col_n];
        }
        double& operator[](int col_n)
        {
            if (col_n >= cols)
            {
                std::cout << "Cannot access [" << row_n << "][" << col_n << "] element of matrix\n";
                // wanna dump();
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
        cols = rhs.cols;
        rows = rhs.rows;

        storage = new double[rhs.cols * rhs.rows];
        rows_arr = new double*[rhs.rows];

        for (int i = 0; i != rhs.rows; ++i)
        {
            rows_arr[i] = storage + i * rhs.cols;
        }

        for (int i = 0, size = rhs.cols * rhs.rows; i != size; ++i)
        {
            storage[i] = rhs.storage[i];
        }
    }

    matrix_t(matrix_t&& rhs) // move constructor
    {
        cols = rhs.cols;
        rows = rhs.rows;
        storage = rhs.storage;
        rows_arr = rhs.rows_arr;
    }

    ~matrix_t() // destructor
    {
        delete[] rows_arr;
        delete[] storage;
    }

    matrix_t& operator=(matrix_t&& rhs) // move assign
    {
        if (this == &rhs)
        {
            return *this;
        }

        delete[] rows_arr;
        delete[] storage;

        cols = rhs.cols;
        rows = rhs.rows;
        storage = rhs.storage;
        rows_arr = rhs.rows_arr;

        rhs.rows = 0;
        rhs.cols = 0;
        rhs.storage = NULL;
        rhs.rows_arr = NULL;
        return *this;
    }

    matrix_t& operator=(const matrix_t& rhs) // copy assign
    {
        if (this == &rhs)
        {
            return *this;
        }
        if (cols != rhs.cols ||  rows != rhs.rows)
        {
            delete[] rows_arr;
            delete[] storage;

            cols = rhs.cols;
            rows = rhs.rows;
            storage = new double[rhs.cols * rhs.rows];
            rows_arr = new double*[rhs.rows];
        }

        for (int i = 0; i != rhs.rows; ++i)
        {
            rows_arr[i] = storage + i * rhs.cols;
        }

        return *this;
    }


    ProxyRow operator[](unsigned row_n)
    {
        if (row_n >= rows)
        {
            std::cout << "Cannot access [" << row_n << "] row of matrix:\n";
            dump();
            exit(1);
        }

        ProxyRow row;
        row.cols = cols;
        row.row_n = row_n;
        row.row = rows_arr[row_n];
        return row;
    }


    void clear()
    {
        for (int i = 0, size = cols * rows; i != size; ++i)
        {
            storage[i] = 0;
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



}; // end of matrix_t

} // end of namespace matrices
