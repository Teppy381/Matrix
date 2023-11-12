#include <gtest/gtest.h>
#include "../matrices.hpp"
#include <random>


TEST(Matrix, ClearCheck)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> size_distribution(10, 100);

    unsigned rows = size_distribution(gen);
    unsigned cols = size_distribution(gen);
    matrices::matrix_t<double> M{rows, cols};

    M.clear();

    for (int i = 0; i != rows; ++i)
    {
        for (int j = 0; j != cols; ++j)
        {
            EXPECT_DOUBLE_EQ(M[i][j], 0);
        }
    }
}

TEST(Matrix, DoubleDeterminantCheck)
{
    unsigned tests_amount = 10;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distribution(-50.0, 50.0);
    std::uniform_real_distribution<double> size_distribution(10, 100);

    for (int t = 0; t != tests_amount; ++t)
    {
        unsigned size = size_distribution(gen);
        matrices::matrix_t<double> M{size};

        double det = 1.0;
        for (int i = 0; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                if (i == j)
                {
                    M[i][j] = distribution(gen);
                    det *= M[i][j];
                }
            }
        }

        for (int i = 1; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[i - 1][j];
            }
        }

        for (int i = 1; i < size/2; ++i)
        {
            double num = distribution(gen);
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[size - i][j] * num;
            }
        }

        EXPECT_NEAR(1, det / M.calc_determinant(), 0.0000000000001);
    }
}

TEST(Matrix, IntegerDeterminantCheck)
{
    unsigned tests_amount = 10;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distribution(-5.0, 5.0);
    std::uniform_real_distribution<double> size_distribution(2.0, 10.0);

    for (int t = 0; t != tests_amount; ++t)
    {
        unsigned size = size_distribution(gen);
        matrices::matrix_t<int> M{size};

        double det = 1.0;
        for (int i = 0; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                if (i == j)
                {
                    M[i][j] = (int) distribution(gen);
                    det *= M[i][j];
                }
            }
        }

        for (int i = 1; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[i - 1][j];
            }
        }

        for (int i = 1; i < size/2; ++i)
        {
            int num = (int) distribution(gen);
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[size - i][j] * num;
            }
        }

        EXPECT_EQ((int) det, M.calc_determinant());
    }
}

TEST(DecomposedMatrix, DoubleDeterminantCheck)
{
    unsigned tests_amount = 10;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distribution(-50.0, 50.0);
    std::uniform_real_distribution<double> size_distribution(10, 100);

    for (int t = 0; t != tests_amount; ++t)
    {
        unsigned size = size_distribution(gen);
        matrices::matrix_t<double> M{size};

        double det = 1.0;
        for (int i = 0; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                if (i == j)
                {
                    M[i][j] = distribution(gen);
                    det *= M[i][j];
                }
            }
        }

        for (int i = 1; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[i - 1][j];
            }
        }

        for (int i = 1; i < size/2; ++i)
        {
            double num = distribution(gen);
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[size - i][j] * num;
            }
        }
        matrices::decomposed_matrix decomp{M};
        EXPECT_NEAR(1, det / decomp.determinant, 0.0000000000001);
    }
}

TEST(DecomposedMatrix, IntegerDeterminantCheck)
{
    unsigned tests_amount = 10;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distribution(-5.0, 5.0);
    std::uniform_real_distribution<double> size_distribution(2.0, 10.0);

    for (int t = 0; t != tests_amount; ++t)
    {
        unsigned size = size_distribution(gen);
        matrices::matrix_t<int> M{size};

        double det = 1.0;
        for (int i = 0; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                if (i == j)
                {
                    M[i][j] = (int) distribution(gen);
                    det *= M[i][j];
                }
            }
        }

        for (int i = 1; i != size; ++i)
        {
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[i - 1][j];
            }
        }

        for (int i = 1; i < size/2; ++i)
        {
            int num = (int) distribution(gen);
            for (int j = 0; j != size; ++j)
            {
                M[i][j] += M[size - i][j] * num;
            }
        }
        matrices::decomposed_matrix decomp{M};
        EXPECT_EQ((int) det, decomp.determinant);
    }
}
