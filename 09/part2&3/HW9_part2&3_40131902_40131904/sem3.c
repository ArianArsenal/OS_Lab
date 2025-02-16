#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

sem_t chopstick[NUM_PHILOSOPHERS];  // Semaphores representing chopsticks

void *philosopher(void *arg) {
    int id = *((int *)arg);

    while (1) {
        // Thinking
        printf("Philosopher %d is thinking.\n", id);
        sleep(rand() % 3);

        // Picking up chopsticks
        sem_wait(&chopstick[id]);             // Pick up left chopstick
        sem_wait(&chopstick[(id + 1) % NUM_PHILOSOPHERS]); // Pick up right chopstick

        // Eating
        printf("Philosopher %d is eating.\n", id);
        sleep(rand() % 2);

        // Putting down chopsticks
        sem_post(&chopstick[id]);             // Put down left chopstick
        sem_post(&chopstick[(id + 1) % NUM_PHILOSOPHERS]); // Put down right chopstick
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize semaphores
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&chopstick[i], 0, 1);
        ids[i] = i;
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    // Wait for threads to finish (they won't in this example)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Destroy semaphores
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&chopstick[i]);
    }

    return 0;
}
