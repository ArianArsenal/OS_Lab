#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_COUNT 20  // The maximum value of count

sem_t rw_mutex;        // Semaphore to manage writer access
sem_t mutex;           // Semaphore to manage reader count access
int read_count = 0;    // Number of active readers
int count = 0;         // Shared buffer

void *reader(void *arg) {
    int pid = *((int *)arg);

    while (1) {
        sem_wait(&mutex);       // Lock to modify reader count
        read_count++;
        if (read_count == 1) { // First reader locks the buffer
            sem_wait(&rw_mutex);
        }
        sem_post(&mutex);       // Unlock reader count

        // Reading the buffer
        printf("Reader %d reads count: %d\n", pid, count);

        sem_wait(&mutex);
        read_count--;
        if (read_count == 0) { // Last reader unlocks the buffer
            sem_post(&rw_mutex);
        }
        sem_post(&mutex);

        sleep(1);
    }

    return NULL;
}

void *writer(void *arg) {
    int pid = *((int *)arg);

    while (count < MAX_COUNT) {
        sem_wait(&rw_mutex); // Lock the buffer for writing

        count++;
        printf("Writer %d writes count: %d\n", pid, count);

        sem_post(&rw_mutex); // Unlock the buffer
        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t r1, r2, w1;
    int r1_id = 1, r2_id = 2, w1_id = 1;

    // Initialize semaphores
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // Create threads
    pthread_create(&r1, NULL, reader, &r1_id);
    pthread_create(&r2, NULL, reader, &r2_id);
    pthread_create(&w1, NULL, writer, &w1_id);

    // Wait for threads to complete
    pthread_join(w1, NULL);
    pthread_cancel(r1);  // Stop readers after the writer is done
    pthread_cancel(r2);

    // Destroy semaphores
    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    return 0;
}
