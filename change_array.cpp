#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <chrono>
#include <cmath>
 
using namespace std;
 
// Структура, передаваемая в качестве аргумента потока
struct thread_params {
    // Указатель на элемент массива, с которого поток начнёт применять функцию поэлементно
    float* arr_ptr;

    // Количество элементов на поток
    unsigned int array_block;

    // Номер функции
    unsigned int func_num;
};

// Функция, которую применяем над каждым элементом массива 
float func_choice(float input_number, int func_num) {
    float result; 
    switch(func_num) {
        case 1:
            result = sqrt(input_number);
            break;
        case 2:
            result = abs(input_number);
            break;
        case 3:
            result = cos(input_number);
            break;
        case 4:
            result = sin(input_number);
            break;
        case 5:
            result = exp(input_number);
            break;
        case 6:
            result = log(input_number);
            break;
    }
    return result;
}
 
// Функция, которую будет исполнять созданный поток
void* thread_job(void* arg)
{
    int err;
 
    // Преобразуем аргумент к нужному типу
    thread_params* params = (thread_params*) arg;
    
    // Если не было передано аргумента, то завершаем работу потока
    if (params == NULL) {
        cout << "No arg to work in thread" << endl;
        return NULL;
    } else {
        // Применяем функцию поэлементно
        for (unsigned int i = 0; i < params->array_block; i++) {
            *(params->arr_ptr + i) = func_choice(*(params->arr_ptr + i), params->func_num);
        }
    }
    
    return NULL;
}
  
 
int main(int argc, char* argv[]) {
    
    if (argc < 4) {
        cout << "Not enough arguments" << endl;
        return 0;
    }
 
    unsigned int arr_len = atoi(argv[1]);
    unsigned int threads_count = atoi(argv[2]);
    unsigned int func_number = atoi(argv[3]);
 
    if (threads_count > arr_len) {
        cout << "Array length is less than the number of threads" << endl;
        exit(-1);
    }
 
    // Блок для обработки одним потоком
    unsigned int array_block = arr_len / threads_count;
    // Остаток присваивается последнему потоку
    unsigned int remainder = arr_len % threads_count;
 
    // Массив потоков
    pthread_t* thread_arr = new pthread_t[threads_count];
    
    // Массив чисел
    float* test_array = new float[arr_len];

    for (unsigned int i = 0; i < arr_len; i++) {
        test_array[i] = (float)i;
    }
    
    for (unsigned int i = 0; i < arr_len; i++) {
        cout << "arr[" << i << "] = " << test_array[i] << endl;
    }
    cout << endl << "_______________________" << endl;
    
    // Массив параметров потоков
    thread_params* params = new thread_params[threads_count];
    
    int err;
    for (unsigned int i = 0; i < threads_count; i++) {
        // Задаём параметры структуры, которую передадим в поток
        params[i].arr_ptr = test_array + i * array_block;
        params[i].array_block = array_block;
        params[i].func_num = func_number;
        // Если последний поток, то добавим ему на обработку остаток массива
        if (i == threads_count - 1) {
            params[i].array_block += remainder;
        }
 
        err = pthread_create(thread_arr + i, NULL, thread_job, (void*) (params + i));
        if (err != 0) {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }
    }
 
    // Ждём завершения всех дочерних потоков
    for (unsigned int i = 0; i < threads_count; i++) {
        pthread_join(thread_arr[i], NULL);
    }
 
    for (unsigned int i = 0; i < arr_len; i++) {
        cout << "arr[" << i << "] = " << test_array[i] << endl;
    }
    
    delete[] test_array;
    delete[] params;
    delete[] thread_arr;
}
