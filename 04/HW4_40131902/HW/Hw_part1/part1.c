#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define SHM_SIZE sizeof(int) * 2  // Shared memory size to hold two integers
#define COUNT 10                  // Number of random numbers to produce and consume

int main() {
    int shm_id;
    int *shared_memory;

    // Step 1: Create shared memory segment
    shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    // Step 2: Fork a new process
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    
    if (pid == 0) {  // Child process (Consumer)
        // Attach to shared memory
        shared_memory = (int *)shmat(shm_id, NULL, 0);
        if (shared_memory == (int *)-1) {
            perror("shmat failed in consumer");
            exit(1);
        }

        int sum = 0;

        for (int i = 0; i < COUNT; i++) {
            // Wait for producer to write a new number
            while (shared_memory[1] == 0) {
                usleep(100);  // Small delay to prevent busy waiting
            }

            // Read number from shared memory
            int num = shared_memory[0];
            printf("Consumer read: %d\n", num);
            sum += num;

            // Reset shared memory flag
            shared_memory[1] = 0;
        }

        printf("Total Sum: %d\n", sum);

        // Detach shared memory
        shmdt(shared_memory);
    } else {  // Parent process (Producer)
        // Attach to shared memory
        shared_memory = (int *)shmat(shm_id, NULL, 0);
        if (shared_memory == (int *)-1) {
            perror("shmat failed in producer");
            exit(1);
        }

        for (int i = 0; i < COUNT; i++) {
            int num = rand() % 100;  // Generate random number

            // Wait until consumer has read the previous number
            while (shared_memory[1] == 1) {
                usleep(100);  // Small delay to prevent busy waiting
            }

            // Write number to shared memory
            shared_memory[0] = num;
            shared_memory[1] = 1;
            printf("Producer wrote: %d\n", num);
        }

        // Wait for child process to finish
        wait(NULL);

        // Detach and remove shared memory
        shmdt(shared_memory);
        shmctl(shm_id, IPC_RMID, NULL);
    }

    return 0;
}