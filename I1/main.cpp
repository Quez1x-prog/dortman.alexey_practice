#include <cstddef>
#include <limits>
#include <stdexcept>

void free_matrix(int ** matrix, size_t allocated_rows)
{
    if (matrix == nullptr)
    {
        throw std::invalid_argument("Указатель на указатель на матрицу не может быть равен nullptr");
    }
    for (size_t i = 0; i < allocated_rows; ++i)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int ** concat_rows(const int * const * lhs, size_t n1, size_t m1, const int * const * rhs, size_t n2, size_t m2)
{
    if (lhs == nullptr || rhs == nullptr)
    {
        throw std::invalid_argument("Указатель на исходный массив не может быть равен nullptr");
    }
    if (m1 > 0)
    {
        for (size_t i = 0; i < n1; ++i)
        {
            if (lhs[i] == nullptr)
            {
                throw std::invalid_argument("Указатель на строку исходного массива не может быть равен nullptr");
            }
        }
    }
    if (m2 > 0)
    {
        for (size_t i = 0; i < n2; ++i)
        {
            if (rhs[i] == nullptr)
            {
                throw std::invalid_argument("Указатель на строку исходного массива не может быть равен nullptr");
            }
        }
    }
    if (std::numeric_limits<size_t>::max()-m1 < m2)
    {
        throw std::overflow_error("Переполнение при m1 + m2");
    }
    size_t new_m = m1 + m2;
    
    size_t max_n = (n1 > n2) ? n1 : n2;
    size_t min_n = (n1 < n2) ? n1 : n2;

    int ** result = new int*[max_n];

    size_t allocated = 0;
    try
    {
        for (; allocated < min_n; ++allocated)
        {
            result[allocated] = new int[new_m];

            for (size_t j = 0; j < m1; ++j)
            {
                result[allocated][j] = lhs[allocated][j];
            }
            for (size_t j = 0; j < m2; ++j)
            {
                result[allocated][m1 + j] = rhs[allocated][j];
            }
        }
    if (n1 > n2)
        {
            for (; allocated < max_n; ++allocated)
            {
                result[allocated] = new int[m2];
                for (size_t j = 0; j < m2; ++j)
                {
                    result[allocated][j] = rhs[allocated][j];
                }
            }
        }
    }
    catch (...)
    {
        free_matrix(result, allocated);
        throw;
    }
    return result;       
}
