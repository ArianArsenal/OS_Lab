#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void swap(int* xp, int* yp){
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubbleSort(int arr[], int n){
    int i, j;
    bool swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = false;
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }
}

int main(){

    int process_count;
    scanf("%d",&process_count);
    
    int CPUburst[process_count];
    int waitTime[process_count];
    int cpuburst;
    
    for (int i = 0; i < process_count; i++)
    {
        scanf("%d",&cpuburst);
        CPUburst[i] = cpuburst;  
        waitTime[i] = 0;  
        //CPUburst[0] = 0; 
    }

    bubbleSort(CPUburst,process_count);
    for (int i = 1; i < process_count; i++)
    {
        waitTime[i] = waitTime[i-1] + CPUburst[i-1];
    }
    
    int sum = 0;
    printf("-------------------");

    //test print
    for (int i = 0; i < process_count; i++)
    {
        //printf("%d",CPUburst[i]);
        sum += waitTime[i];
        printf("\n%d, WaitTime:%d\n",i+1,waitTime[i]);
    }

    double averageWaiting = (double)sum/process_count;

    printf("-------------------");
    printf("\nAverage :%lf\n",averageWaiting);
    
    return 0;
}