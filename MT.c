#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

#define MAX_FILENAME_LENGTH 256
#define NUM_THREADS_DEFAULT 4

long long int finalSum = 0,  finalMin = LLONG_MAX, finalMax = LLONG_MIN;
pthread_mutex_t sumMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t minMaxMutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct ThreadData {
    int threadId;
    long long int* dataArray;
    size_t start;
    size_t end;
} ThreadData;

void* sumFunction(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    long long int partialSum = 0;

    for (size_t i = threadData->start; i < threadData->end; i++) {
        partialSum += threadData->dataArray[i];
    }

    pthread_mutex_lock(&sumMutex);
    finalSum += partialSum;
    pthread_mutex_unlock(&sumMutex);

    // printf("Thread %d: partial sum = %lld\n", threadData->threadId, partialSum);

    pthread_exit(NULL);
}

void* minMaxFunction(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    long long int partialMin = LLONG_MAX, partialMax = LLONG_MIN;

    for (size_t i = threadData->start; i < threadData->end; i++) {
        if (threadData->dataArray[i] < partialMin) {
            partialMin = threadData->dataArray[i];
        }

        if (threadData->dataArray[i] > partialMax) {
            partialMax = threadData->dataArray[i];
        }
    }

    pthread_mutex_lock(&minMaxMutex);

    if (partialMin < finalMin) {
        finalMin = partialMin;
    }

    if (partialMax > finalMax) {
        finalMax = partialMax;
    }

    pthread_mutex_unlock(&minMaxMutex);

    // printf("Thread %d: partial min = %lld, partial max = %lld\n", threadData->threadId, partialMin, partialMax);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    long long int finalised_sum;
    char programName[MAX_FILENAME_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    int numThreads = NUM_THREADS_DEFAULT;

    if (argc < 3) {
        fprintf(stderr, "Number of arguments for %s are less than 3\n", argv[0]);
        return 1;
    }

    snprintf(programName, sizeof(programName), "%s", argv[1]);
    snprintf(filename, sizeof(filename), "%s", argv[2]);

    if (argc >= 4) {
        numThreads = atoi(argv[3]);
    } else {
        numThreads = NUM_THREADS_DEFAULT;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    long long int value;
    size_t dataSize = 0;
    while (fscanf(file, "%lld", &value) == 1) {
        dataSize++;
    }

    long long int* dataArray = (long long int*)malloc(dataSize * sizeof(long long int));
    if (!dataArray) {
        perror("Memory allocation error");
        fclose(file);
        return 1;
    }

    fseek(file, 0, SEEK_SET);

    for (size_t i = 0; i < dataSize; i++) {
        fscanf(file, "%lld", &dataArray[i]);
    }

    fclose(file);

    struct timespec start_time_sum, end_time_sum, start_time_minmax, end_time_minmax;

    double elapsed_time_sum = 0.0;
    double elapsed_time_minmax = 0.0;

    for (int j = 0; j < 5; j++) {
        size_t chunkSize = dataSize / numThreads;
        size_t remainder = dataSize % numThreads;
        size_t startIndex = 0;

        pthread_t sumThreads[numThreads];
        ThreadData sumThreadData[numThreads];

        pthread_t minMaxThreads[numThreads];
        ThreadData minMaxThreadData[numThreads];

        clock_gettime(CLOCK_MONOTONIC, &start_time_sum);

        for (int i = 0; i < numThreads; i++) {
            sumThreadData[i].dataArray = dataArray;
            sumThreadData[i].start = startIndex;
            sumThreadData[i].end = startIndex + chunkSize;
            if ((size_t)i < remainder) {
                sumThreadData[i].end += 1;
            }
            sumThreadData[i].threadId = i;

            if (pthread_create(&sumThreads[i], NULL, sumFunction, (void*)&sumThreadData[i]) != 0) {
                perror("Error: Thread was not created properly");
                free(dataArray);
                exit(1);
            }

            startIndex += sumThreadData[i].end - sumThreadData[i].start;
        }

        for (int i = 0; i < numThreads; i++) {
            pthread_join(sumThreads[i], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time_sum);

        clock_gettime(CLOCK_MONOTONIC, &start_time_minmax);

        startIndex = 0;
        for (int i = 0; i < numThreads; i++) {
            minMaxThreadData[i].dataArray = dataArray;
            minMaxThreadData[i].start = startIndex;
            minMaxThreadData[i].end = startIndex + chunkSize;
            if ((size_t)i < remainder) {
                minMaxThreadData[i].end += 1;
            }
            minMaxThreadData[i].threadId = i;

            if (pthread_create(&minMaxThreads[i], NULL, minMaxFunction, (void*)&minMaxThreadData[i]) != 0) {
                perror("Error: Thread was not created properly");
                free(dataArray);
                exit(1);
            }

            startIndex += minMaxThreadData[i].end - minMaxThreadData[i].start;
        }

        for (int i = 0; i < numThreads; i++) {
            pthread_join(minMaxThreads[i], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time_minmax);

        elapsed_time_sum += (end_time_sum.tv_sec - start_time_sum.tv_sec) +
                            (end_time_sum.tv_nsec - start_time_sum.tv_nsec) / 1e9;

        elapsed_time_minmax += (end_time_minmax.tv_sec - start_time_minmax.tv_sec) +
                               (end_time_minmax.tv_nsec - start_time_minmax.tv_nsec) / 1e9;
        if (j == 0)finalised_sum = finalSum;
    }

    printf("Average elapsed time for Sum in seconds is %f\n", elapsed_time_sum / 5);
    printf("Average elapsed time for Min/Max in seconds is %f\n", elapsed_time_minmax / 5);

    printf("Program Name: %s\n", programName);
    printf("Total Sum: %lld\n", finalised_sum);
    printf("Minimum: %lld\n", finalMin);
    printf("Maximum: %lld\n", finalMax);

    free(dataArray);

    return 0;
}
