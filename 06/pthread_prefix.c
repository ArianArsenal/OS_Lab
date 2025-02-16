#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 500000
int array[SIZE];
int secondArray[SIZE];

void first() {
    for (int i = 1; i < SIZE / 2; i++) {
        secondArray[i] = secondArray[i - 1] + array[i];
    }
}

void second() {
    for (int i = SIZE / 2; i < SIZE; i++) {
        secondArray[i] = secondArray[i - 1] + array[i];
    }
}

int main() {
    // Initialize the array with values from 0 to SIZE-1
    for (int i = 0; i < SIZE; i++) {
        array[i] = i;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

   

    pthread_t thread_one, thread_two;

    pthread_create(&thread_one, NULL, (void *)first, NULL);
    pthread_create(&thread_two, NULL, (void *)second, NULL);

    pthread_join(thread_one, NULL);
    pthread_join(thread_two, NULL);

    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    // Calculate elapsed time in seconds
    

    // Print results (optional)
    printf("After Two Threads\n");
    for (int i = 0; i < SIZE; i++) {
        int tmp = 0;
        if (i < SIZE / 2) {
            tmp += array[i];
            printf("%d\n", secondArray[i] + array[0]);
        } else {
            printf("%d\n", secondArray[i] + tmp + array[0]);
        }
    }

    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}
