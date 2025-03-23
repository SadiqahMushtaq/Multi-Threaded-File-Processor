#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX_FILENAME_LENGTH 256
#define LLONG_MAX -1000000000
#define LLONG_MIN 1000000000

int main(int argc, char *argv[]) {
    long long int final_sum;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <program_name> <filename>\n", argv[0]);
        return 1;
    }

    char *programName = argv[1];
    char *filename = argv[2];

    long long int minimum = LLONG_MIN;
    long long int maximum = LLONG_MAX;
    long long int sum = 0;

    // Define variables for timing
    clock_t start_time, end_time;
    double elapsed_time_minmax = 0.0;
    double elapsed_time_sum = 0.0;

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    long long int value;

     // Count the number of long long integers in the file
    size_t count = 0;
    while (fscanf(file, "%lld", &value) == 1) {
        // printf("Value: %lld\n", value);
        count++;
    }

    // Allocate memory for the dynamic array
    long long int *dataArray = (long long int *)malloc(count * sizeof(long long int));
    if (!dataArray) {
        perror("Memory allocation error");
        fclose(file);
        return 1;
    }

    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Read data from the file into the dynamic array
    for(size_t i = 0; i < count; i++) {
        fscanf(file, "%lld", &dataArray[i]);
    }


    for (int j = 0; j < 5; j++) {
        // Start the timer for min_max
        start_time = clock();
        // Read data from the file into the dynamic array for min_max
        for (size_t i = 0; i < count; i++) {
            if (dataArray[i] < minimum) {
                minimum = dataArray[i];
            }
            if (dataArray[i] > maximum) {
                maximum = dataArray[i];
            }
        }

        // Stop the timer for min_max
        end_time = clock();
        elapsed_time_minmax += (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Start the timer for sum
        start_time = clock();
        // Read data from the file into the dynamic array for sum
        for (size_t i = 0; i < count; i++) {
            sum += dataArray[i];
        }

        // Stop the timer for sum
        end_time = clock();
        elapsed_time_sum += (double)(end_time - start_time) / CLOCKS_PER_SEC;

        if(j == 0) final_sum = sum;

    }
    free(dataArray);
    fclose(file);


    // Calculate average times
    double avg_elapsed_time_minmax = elapsed_time_minmax / 5;
    double avg_elapsed_time_sum = elapsed_time_sum / 5;

    // Print the results
    printf("Program Name: %s\n", programName);
    printf("Minimum: %lld\n", minimum);
    printf("Maximum: %lld\n", maximum);
    printf("Sum: %lld\n", final_sum);
    printf("Average Elapsed Time for Min/Max: %f seconds\n", avg_elapsed_time_minmax);
    printf("Average Elapsed Time for Sum: %f seconds\n", avg_elapsed_time_sum);

    return 0;
}
