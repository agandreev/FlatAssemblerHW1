//c++ main.cpp -fopenmp
#include <functional>
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

//поэлементный рассчет матричного умножения
void *matrixElementCompution(int **matrix1, int **matrix2, int **resultMatrix, int N, int i, int j) {

    //воспользуемся формулой получения одного элемента произведения двух матриц
    for (int k = 0; k < N; ++k) {
        resultMatrix[i][j] +=
                matrix1[i][k] * matrix2[k][j];
    }

    return NULL;
}

//вывод матрицы
void PrintMatrix(const char *title, int **matrix, int N) {
    printf("------------------\n%s\n------------------\n", title);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    //ввод размерностей матрицы
    int N;
    printf("Enter size of quadratic matrix's:");
    cin >> N;
    if (N <= 0) {
        return 0;
    }

    //ввод количества потоков
    int threadQuantity;
    printf("Enter quantity of threads:");
    cin >> threadQuantity;
    if (threadQuantity <= 0) {
        return 0;
    }

    //выделяем память под двумерные массивы
    int **matrix1 = (int **) malloc(N * sizeof(int *));
    int **matrix2 = (int **) malloc(N * sizeof(int *));
    int **resultMatrix = (int **) malloc(N * sizeof(int *));

    //выделяем память под элементы матриц
    for (int i = 0; i < N; i++) {
        matrix1[i] = (int *) malloc(N * sizeof(int));
        matrix2[i] = (int *) malloc(N * sizeof(int));
        resultMatrix[i] = (int *) malloc(N * sizeof(int));
    }

    //берем модуль по 10 для простоты проверки
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix1[i][j] = rand() % 10;
            matrix2[i][j] = rand() % 10;
            resultMatrix[i][j] = 0;
        }
    }
    int i;
    //задаем кол-во потоков
#pragma omp shared(matrix1, matrix2, resultMatrix, N) private(i)
    {
        // Распределяем циклы по потокам. При этом все потоки будут обращаться
        // к одной и той же области памяти для matrix1, matrix2, resultMatrix.
        // И каждый поток будет иметь свою копию переменной i, N
#pragma omp parallel for num_threads(threadQuantity) default(shared)
        // N*N - кол-во матричных элементов, которые должны быть посчитаны,
        // соотвественно каждый поток отвечает за подсчет своего элемента.
        // i / N - номер строки, i % N - номер столбца.
        for (i = 0; i < N * N; ++i) {
            printf("[%d, %d] - thread(%d)\n", i / N, i % N, omp_get_thread_num());
            matrixElementCompution(matrix1, matrix2, resultMatrix, N, i / N, i % N);
        }
        // Точка синхронизации. Дожидаемся, когда все потоки дойдут до этого момента и
        // приступаем к выводу матриц в консоль и чистке памяти.
#pragma omp barrier
        {
            PrintMatrix("First matrix", matrix1, N);
            PrintMatrix("Second matrix", matrix2, N);
            PrintMatrix("Result of multiplying", resultMatrix, N);
            for (int i = 0; i < N; i++) {
                free(matrix1[i]);
                free(matrix2[i]);
                free(resultMatrix[i]);
            }
            free(matrix1);
            free(matrix2);
            free(resultMatrix);
        }
    }
    return 0;
}

