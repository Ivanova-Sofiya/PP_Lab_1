#include <cstdlib>
#include <iostream>
#include <cstring>
#include <chrono>
#include <pthread.h>

using namespace std;

// Структура, передаваемая в качестве аргумента потока
struct thread_params {
    // Количество операций внутри потока
    unsigned int operations_count;

    // Нужно ли выводить в консоль информацию об атрибутах потока
    bool enable_cout;

    // Время работы внутри потока
    chrono::microseconds work_time;
};


/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    auto begin = chrono::steady_clock::now();
    int err;

    // Преобразуем аргумент к нужному типу
    thread_params *params = (thread_params *) arg;

    // Если не было передано аргумента, то завершаем работу потока
    if (params == NULL) {
        cout << "No arguments";
        return NULL;
    }

    // Из структуры-аргумента получаем число операций
    int operation_count = params->operations_count;

    double number = 0.0;
    for (unsigned int i = 0; i < operation_count; i++) {
        number += 1.22222;
    }

    if (params->enable_cout == true) {
        // Получаем индентефикатор текущего потока
        pthread_t thread;
        thread = pthread_self();
 
        // Получаем переменную с информацией об атрибутах текущего потока
        pthread_attr_t attr;
        pthread_getattr_np(thread, &attr);
        if (err != 0) {
            cout << "Cannot get thread attributes: " << strerror(err) << endl;
            exit(-1);
        }
 
        // Из структуры получаем:
        // + Тип создаваемого потока
        int detach_state;
        pthread_attr_getdetachstate(&attr, &detach_state);
        if (err != 0) {
            cout << "Cannot get detachet state: " << strerror(err) << endl;
            exit(-1);
        }
 
        // + Размер охранной зоны, байт
        size_t guard_size;
        pthread_attr_getguardsize(&attr, &guard_size);
        if (err != 0) {
            cout << "Cannot get guard size: " << strerror(err) << endl;
            exit(-1);
        }
        
        // + Размер стека, байт
        size_t stack_size;

        // + Адрес стека
        void * stack_addr;
        pthread_attr_getstack(&attr, &stack_addr, &stack_size);
        if (err != 0) {
            cout << "Cannot get stack: " << strerror(err) << endl;
            exit(-1);
        }
 
        // Освобождаем память, занятую под хранение атрибутов потока
        pthread_attr_destroy(&attr);
 
        cout << "*****Thread info*****" << endl;
        cout << "Detach state: ";
        if (detach_state == PTHREAD_CREATE_JOINABLE) {
            cout << "joinable";
        } else {
            cout << "detached";
        }
        cout << endl;
 
        cout << "Guard size: " << guard_size << endl;
        cout << "Stack address: " << stack_addr << endl;
        cout << "Stack size: " << stack_size << endl;
    }

    
    auto end = chrono::steady_clock::now();
 
    auto diff_time = chrono::duration_cast<std::chrono::microseconds>(end - begin);
    params->work_time = diff_time;
    
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
            cout << "Not enough arguments" << endl;
            return 0;
        }

    // Определяем переменные: идентификатор потока и код ошибки
    pthread_t thread;
    int err;

    // Количество потоков
    int threads_count = atoi(argv[1]);

    // Инициализируем структуру-аргумент
    thread_params params;
    // Число операций
    params.operations_count = 100;
    params.enable_cout = true;


    for (int i = 0; i < threads_count; i++) {

        auto begin = chrono::steady_clock::now();
        // Создаём поток
        err = pthread_create(&thread, NULL, thread_job, (void*) &params);
        // Если при создании потока произошла ошибка, выводим
        // сообщение об ошибке и прекращаем работу программы
        if(err != 0) {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }

        err = pthread_join(thread, NULL);
        if (err != 0) {
            cout << "Cannot join a thread: " << strerror(err) << endl;
            exit(-1);
        }

        
        auto end = chrono::steady_clock::now();
        auto diff_time = chrono::duration_cast<std::chrono::microseconds>(end - begin);
 
        // cout << params.operations_count << " operations: " << endl
        //      << "inner time: " << params.work_time.count() << endl
        //      << "outer time: " << diff_time.count() << endl
        //      << "time difference: " << diff_time.count() - params.work_time.count() << endl << endl;
        
        params.operations_count *= 20;
    }

    // Проинициализируем переменную для хранения атрибутов потока
    pthread_attr_t thread_attr;
    err = pthread_attr_init(&thread_attr);
    if (err != 0)
    {
        cout << "Cannot create thread attribute: " << strerror(err) << endl;
        exit(-1);
    }

    
    // Поменяем размер стека
    err = pthread_attr_setstacksize(&thread_attr, 1024 * 1024);
    if (err != 0)
    {
        cout << "Setting stack size attribute failed: " << strerror(err)
            << endl;
        exit(-1);
    }
 
    // Установим необходимость вывода информации об атрибутах потока
    // params.enable_cout = true;
    cout << endl << "Change thread attribute: " << endl << endl;
    // Создаём поток с измененными атрибутами
    err = pthread_create(&thread, &thread_attr, thread_job, (void*) &params);
    if (err != 0)
    {
        cout << "Cannot create a thread: " << strerror(err) << endl;
        exit(-1);
    }
 
    // Освобождаем память, занятую под хранение атрибутов потока
    pthread_attr_destroy(&thread_attr);


    // Ожидаем завершения созданного потока перед завершением
    // работы программы
    pthread_exit(NULL);
} 