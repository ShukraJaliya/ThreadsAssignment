#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <my_thread.h>
#include <sys/time.h>
#include <assert.h>

#define NUM_THREADS 3
#define NUM_NUMBERS 1000000

struct ThreadData {
    char thread_id;
    double execution_time;
};

double Time_GetSeconds() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)((double)t.tv_sec + (double)t.tv_usec / 1e6);
}

void* threadFunction(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char thread_id = data->thread_id;
    double start_time = Time_GetSeconds();

    std::ifstream infile("Data.txt");
    if (!infile.is_open()) {
        std::cerr << "Error opening Data.txt" << std::endl;
        pthread_exit(NULL);
    }

    std::string output_file = std::string("Data_Thread_") + thread_id + ".txt";
    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << "Error opening " << output_file << std::endl;
        infile.close();
        pthread_exit(NULL);
    }

    double sum = 0;
    int count = 0;
    int number;
    while (infile >> number) {
        outfile << number << std::endl;
        sum += number;
        count++;
    }

    double average = sum / count;
    std::cout << "Thread " << thread_id << " average: " << average << std::endl;

    infile.close();
    outfile.close();

    double end_time = Time_GetSeconds();
    data->execution_time = (end_time - start_time) * 1000; // convert to milliseconds
    std::cout << "Thread " << thread_id << " execution time: " << data->execution_time << " ms" << std::endl;

    pthread_exit(NULL);
}

int main() {
    // Open the data file and write random numbers into it
    std::ofstream data_file("Data.txt");
    if (!data_file.is_open()) {
        std::cerr << "Error opening Data.txt" << std::endl;
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);

    for (int i = 0; i < NUM_NUMBERS; ++i) {
        data_file << dis(gen) << std::endl;
    }
    data_file.close();

    // Create threads
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS] = { {'A', 0}, {'B', 0}, {'C', 0} };

    double main_start_time = Time_GetSeconds();

    for (int i = 0; i < NUM_THREADS; ++i) {
        int rc = pthread_create(&threads[i], NULL, threadFunction, (void*)&thread_data[i]);
        if (rc) {
            std::cerr << "Error: unable to create thread " << rc << std::endl;
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    double main_end_time = Time_GetSeconds();
    double main_execution_time = (main_end_time - main_start_time) * 1000; // convert to milliseconds

    std::cout << "Total execution time: " << main_execution_time << " ms" << std::endl;

    return 0;
}
