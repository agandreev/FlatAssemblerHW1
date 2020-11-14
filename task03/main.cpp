#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

using namespace std;

//специальная структура для данных потока
typedef struct {
    int row; //номер строки получаемого элемента
    int column; //номер столбца получаемого элемента
    int len; //размерность квадратной матрицы

    //указатели на матрицы
    int** array1;
    int** array2;
    int** resArr;
} pthrData;

//поэлементный рассчет матричного умножения
void* matrixElementCompution(void* thread_data){
    //получаем структуру с данными
    pthrData *data = (pthrData*) thread_data;

    //воспользуемся формулой получения одного элемента произведения двух матриц
    for (int i = 0; i < data->len; ++i) {
        data->resArr[data->row][data->column] +=
                data->array1[data->row][i] * data->array2[i][data->column];
    }
    
    return NULL;
}

//вывод матрицы
void PrintMatrix(const char* title, int** matrix, int N) {
    printf("------------------\n%s\n------------------\n", title);
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(){
    int N;
    printf("Enter size of quadratic matrix's:");
    cin >> N;
    if (N <= 0){
        return 0;
    }

    int threadQuantity;
    printf("Enter quantity of threads:");
    cin >> threadQuantity;
    if (threadQuantity <= 0){
        return 0;
    }

    //выделяем память под двумерные массивы
    int** matrix1 = (int**) malloc(N * sizeof(int*));
    int** matrix2 = (int**) malloc(N * sizeof(int*));
    int** resultMatrix = (int**) malloc(N * sizeof(int*));

    //выделяем память под элементы матриц
    for(int i = 0; i < N; i++){
        matrix1[i] = (int*) malloc(N * sizeof(int));
        matrix2[i] = (int*) malloc(N * sizeof(int));
        resultMatrix[i] = (int*) malloc(N * sizeof(int));
    }

    //берем модуль по 10 для простоты проверки
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            matrix1[i][j] = rand() % 10;
            matrix2[i][j] = rand() % 10;
            resultMatrix[i][j] = 0;
        }
    }

    //выделяем память под массив идентификаторов потоков
    pthread_t* threads = (pthread_t*) malloc(N * N * sizeof(pthread_t));
    //сколько потоков - столько и структур с потоковых данных
    pthrData* threadData = (pthrData*) malloc(N * N * sizeof(pthrData));

    //инициализируем структуры потоков
    int threadNumber;
    for(int i = 0; i < N; ++i){
        for (int j = 0; j < N; ++j) {
            threadNumber = (i*N + j) % threadQuantity; //номер потока [0..threadQuantity)
            threadData[threadNumber].row = i; //номер элемента по строке
            threadData[threadNumber].column = j; //номер элемента по столбцу
            threadData[threadNumber].len = N; //длина строк/столбцов
            threadData[threadNumber].array1 = matrix1;
            threadData[threadNumber].array2 = matrix2;
            threadData[threadNumber].resArr = resultMatrix;

            //считаем каждый элемент матрицы в отдельном потоке
            pthread_create(&(threads[threadNumber]), NULL,
                           matrixElementCompution, &threadData[threadNumber]);

            //если все потоки использованы, то ждем, когда они закончат вычисления
            if (threadNumber == (threadQuantity - 1)){
                for (int k = 0; k < threadQuantity; ++k) {
                    pthread_join(threads[k], NULL);
                }
            }
        }
    }

    //ожидаем выполнение остальных потоков, на случай, если i*N + j некратно кол-ву потоков
    for (int k = 0; k < threadQuantity; ++k) {
        pthread_join(threads[k], NULL);
    }

    //вывод
    PrintMatrix("First matrix", matrix1, N);
    PrintMatrix("Second matrix", matrix2, N);
    PrintMatrix("Result of multiplying", resultMatrix, N);

    //освобождаем память
    free(threads);
    free(threadData);
    for(int i = 0; i < N; i++){
        free(matrix1[i]);
        free(matrix2[i]);
        free(resultMatrix[i]);
    }
    free(matrix1);
    free(matrix2);
    free(resultMatrix);
    return 0;
}