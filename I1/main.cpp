#include <iostream>
#include <cstddef>
#include <limits>
#include <stdexcept>

void free_matrix(int ** matrix, size_t allocated_rows)
{
    if (matrix == nullptr)
    {
        return;
    }
    for (size_t i = 0; i < allocated_rows; ++i)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int ** create_matrix(size_t n, size_t m)
{
    const size_t max_size = std::numeric_limits<size_t>::max();

    if (n > max_size / sizeof(int*))
    {
        throw std::overflow_error("Переполнение размера памяти для массива указателей");
    }

    if (m > max_size / sizeof(int))
    {
        throw std::overflow_error("Переполнение размера памяти для строки матрицы");
    }

    if (n > 0 && m > max_size / n)
    {
        throw std::overflow_error("Переполнение общего количества элементов матрицы");
    }

    int ** matrix = new int*[n];
    size_t allocated = 0;
    try
    {
        for (; allocated < n; ++allocated)
        {
            matrix[allocated] = new int[m];
        }
    }
    catch (...)
    {
        free_matrix(matrix, allocated);
        throw;
    }
    return matrix;
}

bool input_matrix(int * const * matrix, size_t n, size_t m)
{
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < m; ++j)
        {
            if (!(std::cin >> matrix[i][j]))
            {
                return false;
            }
        }
    }
    return true;
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
                throw std::invalid_argument("Указатель на строку первого массива равен nullptr");
            }
        }
    }
    if (m2 > 0)
    {
        for (size_t i = 0; i < n2; ++i)
        {
            if (rhs[i] == nullptr)
            {
                throw std::invalid_argument("Указатель на строку второго массива равен nullptr");
            }
        }
    }

    const size_t max_size = std::numeric_limits<size_t>::max();

    if (max_size - m1 < m2)
    {
        throw std::overflow_error("Переполнение при сложении m1 + m2");
    }
    size_t new_m = m1 + m2;

    if (new_m > max_size / sizeof(int))
    {
        throw std::overflow_error("Переполнение размера памяти для объединенной строки");
    }

    size_t max_n = n2;
    size_t min_n = n1;
    if (n1 > n2)
    {
        max_n = n1;
        min_n = n2;
    }
    if (max_n > max_size / sizeof(int*))
    {
        throw std::overflow_error("Переполнение размера памяти для массива указателей результата");
    }
    if (min_n > 0 && new_m > max_size / min_n)
    {
        throw std::overflow_error("Переполнение количества элементов в общей части матриц");
    }

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
            if (m1 > max_size / sizeof(int))
            {
                throw std::overflow_error("Переполнение размера памяти для остатка строк первой матрицы");
            }
            for (; allocated < max_n; ++allocated)
            {
                result[allocated] = new int[m1];
                for (size_t j = 0; j < m1; ++j)
                {
                    result[allocated][j] = lhs[allocated][j];
                }
            }
        }
        else if (n2 > n1)
        {
            if (m2 > max_size / sizeof(int))
            {
                throw std::overflow_error("Переполнение размера памяти для остатка строк второй матрицы");
            }
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

void print_jagged_result(const int * const * res, size_t n1, size_t m1, size_t n2, size_t m2)
{
    if (std::numeric_limits<size_t>::max() - m1 < m2)
    {
        throw std::overflow_error("Переполнение при сложении m1 + m2 в выводе");
    }
    size_t new_m = m1 + m2;

    size_t max_n = n2;
    size_t min_n = n1;
    if (n1 > n2)
    {
        max_n = n1;
        min_n = n2;
    }

    for (size_t i = 0; i < max_n; ++i)
    {
        size_t current_m = 0;
        if (i < min_n)
        {
            current_m = new_m;
        }
        else if (n1 > n2)
        {
            current_m = m1;
        }
        else
        {
            current_m = m2;
        }

        for (size_t j = 0; j < current_m; ++j)
        {
            std::cout << res[i][j];
            if (j < current_m - 1)
            {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

int main()
{
    size_t n1 = 0;
    size_t m1 = 0;
    if (!(std::cin >> n1 >> m1))
    {
        std::cerr << "Ошибка ввода размеров первой матрицы\n";
        return 1;
    }

    int ** lhs = nullptr;
    try
    {
        lhs = create_matrix(n1, m1);
    }
    catch (...)
    {
        std::cerr << "Не удалось выделить память под первую матрицу (или переполнение размера)\n";
        return 2;
    }

    if (!input_matrix(lhs, n1, m1))
    {
        std::cerr << "Ошибка ввода элементов первой матрицы\n";
        free_matrix(lhs, n1);
        return 1;
    }

    size_t n2 = 0;
    size_t m2 = 0;
    if (!(std::cin >> n2 >> m2))
    {
        std::cerr << "Ошибка ввода размеров второй матрицы\n";
        free_matrix(lhs, n1);
        return 1;
    }

    int ** rhs = nullptr;
    try
    {
        rhs = create_matrix(n2, m2);
    }
    catch (...)
    {
        std::cerr << "Не удалось выделить память под вторую матрицу (или переполнение размера)\n";
        free_matrix(lhs, n1);
        return 2;
    }

    if (!input_matrix(rhs, n2, m2))
    {
        std::cerr << "Ошибка ввода элементов второй матрицы\n";
        free_matrix(rhs, n2);
        free_matrix(lhs, n1);
        return 1;
    }

    int ** result = nullptr;
    try
    {
        result = concat_rows(lhs, n1, m1, rhs, n2, m2);
    }
    catch (...)
    {
        std::cerr << "Ошибка при объединении матриц\n";
        free_matrix(rhs, n2);
        free_matrix(lhs, n1);
        return 2;
    }

    size_t max_n = n2;
    if (n1 > n2)
    {
        max_n = n1;
    }

    print_jagged_result(result, n1, m1, n2, m2);

    free_matrix(result, max_n);
    free_matrix(rhs, n2);
    free_matrix(lhs, n1);

    return 0;
}
