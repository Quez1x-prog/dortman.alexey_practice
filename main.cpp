#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <limits>

void freeMatrix(int** matrix, size_t rows) //Для освобождения памяти, указатель на массив указателей
{
  if (!matrix) //Если nullptr выходим без аварийного завершения
  {
    return;
  }
  for (size_t i = 0; i < rows; ++i)
  {
    std::free(matrix[i]); //Освобождаем одномерную строку (память под int)
  }
  std::free(matrix); //Освобождаем массив указателей (память под int*)
}

int** allocateMatrix(size_t rows, size_t cols) //Выделение памяти
{
  if (rows == 0 || cols == 0)
  {
    return nullptr;
  }

  if (rows > std::numeric_limits< size_t >::max() / sizeof(int*)) //Целочисленное переполнение
  {
    return nullptr;
  }
  int** matrix = static_cast< int** >(std::malloc(rows * sizeof(int*))); //Непрерывный блок памяти под массив из rows указателей на целые числа. Приводим тип из void* в int**
  if (!matrix) //Если система не смогла выделить память то nullptr
  {
    return nullptr;
  }

  if (cols > std::numeric_limits< size_t >::max() / sizeof(int)) //Защита от переполнения
  {
    std::free(matrix);
    return nullptr;
  }

  for (size_t i = 0; i < rows; ++i) //Выделяем память
  {
    matrix[i] = static_cast< int* >(std::malloc(cols * sizeof(int)));
    if (!matrix[i])
    {
      freeMatrix(matrix, i); //Если на i кончается память то исключаем утечку памяти
      return nullptr;
    }
  }

  return matrix;
}

int readMatrix(int** matrix, size_t rows, size_t cols)
{
  for (size_t i = 0; i < rows; ++i)
  {
    for (size_t j = 0; j < cols; ++j)
    {
      if (!(std::cin >> matrix[i][j]))
      {
        return 1; 
      }
    }
  }
  return 0;
}

void printTransposed(const int* const* matrix, size_t rows, size_t cols)
{
  for (size_t j = 0; j < cols; ++j)
  {
    for (size_t i = 0; i < rows; ++i)
    {
      std::cout << matrix[i][j] << (i + 1 == rows ? "" : " "); //Выводим матрицу и расставляем пробелы (если конец строки то не ставим)
    }
    std::cout << '\n';
  }
}

int main()
{
  size_t rows = 0;
  size_t cols = 0;

  if (!(std::cin >> rows >> cols) || rows == 0 || cols == 0)
  {
    return 1;
  }

  int** matrix = allocateMatrix(rows, cols); //Если память выделить не удалось код возврата 2
  if (!matrix)
  {
    return 2;
  }

  if (readMatrix(matrix, rows, cols) != 0) //Если данные некорректные то код возврата 1
  {
    freeMatrix(matrix, rows);
    return 1;
  }

  printTransposed(matrix, rows, cols);

  freeMatrix(matrix, rows); //Освобождаем память и выходим с кодом 0
  return 0;
}
