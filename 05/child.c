#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_CHILDREN 2
#define NUM_ITERATIONS 5e5

void printHistogram(int *hist)
{
    int i, j;
    for (i = 0; i < 25; i++)
    {
        for (j = 0; j < hist[i] * 100 / NUM_ITERATIONS; j++)
        {
            printf("*");
        }
        printf("\n");
    }
}

int main() {
    int shmid;
    int *hist;
    int n_per_child = NUM_ITERATIONS / NUM_CHILDREN;
    int counter;
    pid_t pid;

    // Create a shared memory segment
    shmid = shmget(IPC_PRIVATE, 25 * sizeof(int), IPC_CREAT | 0666);

    hist = (int *)shmat(shmid, NULL, 0);

    for (int i = 0; i < 25; i++) {
        hist[i] = 0;
    }

    srand(time(NULL));

    // Start time measurement
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int child = 0; child < NUM_CHILDREN; child++) {
        pid = fork();

        // printf("Pid :%d\n",getpid());

        if (pid == 0) { // Child process
            
            printf("Pid :%d\n",getpid());
            for (int iter = 0; iter < n_per_child; iter++) {
                counter = 0;
                for (int i = 0; i < 12; i++) {
                    int random_number = rand() % 100;
                    if (random_number >= 49)
                        counter++;
                    else
                        counter--;
                }
                // Avoid race conditions
                __sync_fetch_and_add(&hist[counter + 12], 1);
            }
            shmdt(hist);
            exit(0);
        }
    }

    // Wait for all children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // End time measurement
    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printHistogram(hist);
    printf("Time taken: %f seconds\n", time_taken);

    shmdt(hist);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
