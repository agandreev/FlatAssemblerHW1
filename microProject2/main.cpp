#include <iostream>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <queue>
#include <ctime>

using namespace std;

//блокирует вход в парикмахерскую (дверь в комнату ожидания одна)
pthread_mutex_t queueOrganizer;
//блокирует блок кода, в котором происходит процесс стрижки
pthread_mutex_t barberChair;
//блокирует блок кода парикмахера для того, чтобы тот мог спать
pthread_mutex_t barberry;
//переменная состояния наличия клиентов
pthread_cond_t cameCustomer;
//переменная состояния обозначающая конец стрижки
pthread_cond_t hadCut;
//обозначает конец рабочего дня
bool allCut = false;
//обозначает наличие клиента в комнате ожидания
bool isCame = false;
//обозначает наличие процесса стрижки
bool isCutting = false;
//очередь из номеров клиентов (для наглядности)
queue<long> customersQueue;

//метод для потоков-клиентов
//состоит из двух мьютекс блоков: становление в очередь и стрижка
void* stayInQueue(void *args){
    //блок становления в очередь
    pthread_mutex_lock(&queueOrganizer);
    //интервал входа в парикмахерскую
    this_thread::sleep_for(chrono::milliseconds(rand() % 3000 + 2000));
    printf("%d\tCustomer %d entered.\n", (int)clock(), (long)pthread_self());
    customersQueue.push((long)pthread_self());
    //если парикмахер спит, то разбудить его
    if (!isCame){
        pthread_cond_signal(&cameCustomer);
        isCame = true;
        isCutting = true;
    }
    pthread_mutex_unlock(&queueOrganizer) ;

    //блок стрижки
    pthread_mutex_lock(&barberChair);
    //если парикмахер стрижет, то нужно уснуть
    while (isCutting){
        pthread_cond_wait(&hadCut, &barberChair);
    }
    printf("%d\t%d exit.\n", (int)clock(), (long)pthread_self());
    pthread_mutex_unlock(&barberChair) ;

    return NULL;
}

//метод для потока парикмахера
void* cut(void *args){
    pthread_mutex_lock(&barberry);
    //пока рабочий день не закончен...
    while (!allCut){
        //если нет клиентов в комнате ожидания, то спать
        while (!isCame) {
            printf("%d\tBarber is sleeping...\n", (int)clock());
            pthread_cond_wait(&cameCustomer, &barberry);
        }
        //если есть клиенты в очереди, то стричь
        while(!customersQueue.empty()){
            isCutting = true;
            printf("%d\t%d is cutting...\n", (int)clock(), customersQueue.front());
            this_thread::sleep_for(chrono::milliseconds(rand() % 3000 + 1000));
            printf("%d\t%d is already cut!\n", (int)clock(), customersQueue.front());
            isCutting = false;
            //разбудить клиента
            pthread_cond_signal(&hadCut);
            this_thread::sleep_for(chrono::milliseconds(200));
            customersQueue.pop();
        }
        isCame = false;
    }
    pthread_mutex_unlock(&barberry);
    return NULL;
}

//проверка на число
bool checkString(const char*str) {
    while(*str)  {
        if((*str< '0' || *str > '9')) {
            return false;
        }
        *str++;
    }
    return true;
}

int main() {
    string threadsQuantity;
    cout << "Enter quantity of threads(customers):";
    cin >> threadsQuantity;
    //кол-во потоков-клиентов
    int customersQuantity;
    if (!checkString(threadsQuantity.c_str())){
        cout << threadsQuantity << " is illegal!" << endl;
        return 0;
    }
    customersQuantity = stoi(threadsQuantity);
    if (customersQuantity <= 0){
        cout << "Illegal quantity of threads!" << endl;
        return 0;
    }
    srand(time(0));

    pthread_t customers[customersQuantity];

    pthread_mutex_init(&queueOrganizer, NULL);
    pthread_mutex_init(&barberry, NULL);
    pthread_mutex_init(&barberChair, NULL);
    pthread_cond_init(&cameCustomer, NULL);
    pthread_cond_init(&hadCut, NULL);

    pthread_t barber;
    //запуск потоков клиентов
    pthread_create(&barber, NULL, cut, NULL);
    for (int i = 0; i < customersQuantity; ++i) {
        pthread_create(&customers[i], NULL, stayInQueue, NULL);
    }
    for (int i = 0; i < customersQuantity; ++i) {
        pthread_join(customers[i], NULL);
    }
    //окончание рабочего дня
    allCut = true;
    pthread_join(barber, NULL);

    return 0;
}
